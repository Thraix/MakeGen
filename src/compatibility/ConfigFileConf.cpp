#include "ConfigFileConf.h"

const std::string CONFIG_FILENAME_CONF = "makegen.conf";

#include "../ConfigFile.h"
#include "../FileUtils.h"

#include <algorithm>
#include <fstream>

#define FLAG_NONE 0
#define FLAG_VECTOR 1
#define FLAG_STRING 2
#define FLAG_BOOL 3

ConfigFileConf::ConfigFileConf()
  : outputdir("bin/"), srcdir("src/"), outputname(""), projectname(FileUtils::GetCurrentDirectory()), hFile(""), executable(true), shared(true), generateHFile(false)
{
  // Converts project name (current directory) to lowercase
  // and replace whitespace with underscore
  std::transform(
      projectname.begin(),
      projectname.end(),
      std::back_inserter(outputname),
      [](unsigned char c)
      {
      if(c == ' ')
        return '_';
      return (char)std::tolower(c);
      });

  // Removes all other characters
  outputdir.erase(std::remove_if(
      outputdir.begin(),
      outputdir.end(),
      [](unsigned char c)
      {
      return (c < 'a' || c > 'z') && c != '_';
      }));

  // Add suffix
  outputname += ".out";
}

void ConfigFileConf::CreateXMLFile(const std::string& filepath)
{
  ConfigFileConf conf;
  conf.configPath = filepath;
  unsigned int loadFlag = 0;

  std::vector<std::string>* vec;
  std::string* s;
  bool* b;

  bool isDirectory = false;

  std::ifstream file(filepath + CONFIG_FILENAME_CONF);
  std::string line;

  if(file.is_open())
  {
    // config name, { pointer to memory, isDirectory}
    std::map<std::string, std::pair<std::string*, bool>> strings =
    {
      {"#srcdir", {&conf.srcdir, true}},
      {"#outputdir", {&conf.outputdir, true}},
      {"#outputname", {&conf.outputname, false}},
      {"#projectname", {&conf.projectname, false}},
      {"#hfile", {&conf.hFile, false}},
    };

    // config name, { pointer to memory, isDirectory}
    std::map<std::string, std::pair<std::vector<std::string>*, bool>> vectors =
    {
      {"#libs", {&conf.libs, false}},
      {"#libdirs", {&conf.libdirs, true}},
      {"#includedirs", {&conf.includedirs, true}},
      {"#compileflags", {&conf.flags, false}},
      {"#defines", {&conf.defines, false}},
      {"#dependencies", {&conf.dependencies, true}},
      {"#sourcefiles", {&conf.sourceFiles, false}},
    };

    std::map<std::string, bool*> booleans =
    {
      {"#executable", &conf.executable},
      {"#shared", &conf.shared},
      {"#generatehfile", &conf.generateHFile},
    };

    while(std::getline(file,line))
    {
      if(line == "")
        continue;
      if(line[0]=='#')
      {
        // The format is a bit wacky, but it is this way since we do not want
        // to use map::find for all maps. This way we gain some optimization.
        auto&& itStr{strings.find(line)};
        if(itStr != strings.end())
        {
          s = itStr->second.first;
          isDirectory = itStr->second.second;
          loadFlag = FLAG_STRING;
        }
        else
        {
          auto&& itVec{vectors.find(line)};
          if(itVec != vectors.end())
          {
            vec = itVec->second.first;
            isDirectory = itVec->second.second;
            loadFlag = FLAG_VECTOR;
          }
          else
          {
            auto&& itBool{booleans.find(line)};
            if(itBool != booleans.end())
            {
              b = itBool->second;
              loadFlag = FLAG_BOOL;
            }
            else
            {
              LOG_ERROR("Invalid flag: ", line);
              loadFlag = FLAG_NONE;
            }
          }
        }
      }
      else
      {
        if(loadFlag == FLAG_STRING)
        {
          if(isDirectory && line[line.size()-1] != '/')
            line += '/';
          *s = line;
        }
        else if(loadFlag == FLAG_VECTOR)
        {
          if(isDirectory && line[line.size()-1] != '/')
          {;
            line += '/';
          }
          vec->push_back(line);
        }
        else if(loadFlag == FLAG_BOOL)
        {
          if(line == "true")
            *b = true;
          else
            *b = false;
        }
      }
    }

    LOG_INFO("------ COULDN\'T FIND makegen.xml. BUT FOUND OLD makegen.conf.");
    LOG_INFO("------ GENERATING NEW CONFIGURATION FILE");
    if(conf.hFile == "")
      conf.hFile = conf.projectname+".h";

    XMLObject makegen("makegen", {}, std::map<std::string, std::vector<XMLObject>>{});

    // Version, target and configuration is probably going to be used in the future
    makegen.AddXMLObject(XMLObject("version", {}, "v1.3.2"));
    makegen.AddXMLObject(XMLObject("target", {}, "Release"));

    XMLObject configuration("configuration", {{"name", "Release"}}, std::map<std::string, std::vector<XMLObject>>{});
    configuration.AddXMLObject(XMLObject("projectname", {}, conf.projectname));
    configuration.AddXMLObject(XMLObject("outputname", {}, conf.outputname));
    configuration.AddXMLObject(XMLObject("srcdir", {}, conf.srcdir));
    configuration.AddXMLObject(XMLObject("outputdir", {}, conf.outputdir));
    configuration.AddXMLObject(XMLObject("hfilename", {}, conf.hFile));
    configuration.AddXMLObject(XMLObject("outputtype", {},
          conf.executable ? "executable" : (conf.shared ? "sharedlibrary" : "staticlibrary")));
    configuration.AddXMLObject(XMLObject("generatehfile", {}, conf.generateHFile ? "true" : "false"));

    for(auto it = conf.libs.begin();it != conf.libs.end(); ++it)
      configuration.AddXMLObject({"library",{},*it});
    for(auto it = conf.libdirs.begin();it != conf.libdirs.end(); ++it)
      configuration.AddXMLObject({"librarydir",{},*it});
    for(auto it = conf.includedirs.begin();it != conf.includedirs.end(); ++it)
      configuration.AddXMLObject({"includedir",{},*it});
    for(auto it = conf.defines.begin();it != conf.defines.end(); ++it)
      configuration.AddXMLObject({"define",{},*it});
    for(auto it = conf.flags.begin();it != conf.flags.end(); ++it)
      configuration.AddXMLObject({"cflag",{},*it});
    for(auto it = conf.dependencies.begin();it != conf.dependencies.end(); ++it)
      configuration.AddXMLObject({"dependency",{},*it});
    for(auto it = conf.sourceFiles.begin();it != conf.sourceFiles.end(); ++it)
      configuration.AddXMLObject({"sourcefile",{},*it});

    makegen.AddXMLObject(configuration);
    std::ofstream xmlFile(conf.configPath + "makegen.xml");
    xmlFile << makegen;
  }
}
