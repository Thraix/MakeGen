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
  std::remove_if(
      outputdir.begin(),
      outputdir.end(),
      [](unsigned char c)
      {
      return (c < 'a' || c > 'z') && c != '_';
      });

  // Add suffix
  outputname += ".out";
}

ConfigFile ConfigFileConf::Load(const std::string& filepath)
{
  ConfigFile conf;
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
  }
  if(conf.hFile == "")
    conf.hFile = conf.projectname+".h";

  LOG_INFO("------ COULDN\'T FIND makegen.xml. BUT FOUND OLD makegen.conf.");
  LOG_INFO("------ GENERATING NEW CONFIGURATION FILE");

  conf.Save();
  return conf;
}
