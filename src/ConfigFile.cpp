#include "ConfigFile.h"

#include "FileUtils.h"
#include "compatibility/ConfigFileConf.h"
#include "xml/XML.h"

#include <algorithm>
#include <fstream>

ConfigFile::ConfigFile()
  : outputdir("bin/"), srcdir("src/"), outputname(""), projectname(FileUtils::GetCurrentDirectory()), hFile(projectname+".h"), executable(true), shared(true), generateHFile(false)
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

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath)
{
  std::map<std::string, ConfigFile> loadedConfigs;
  return GetConfigFile(filepath, loadedConfigs);
}

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath, std::map<std::string, ConfigFile>& loadedConfigs)
{
  std::string realPath = FileUtils::GetRealPath(filepath);
  auto it = loadedConfigs.find(realPath);
  if(it != loadedConfigs.end())
  {
    return {};
  }

  bool oldFile = false;
  std::ifstream f(filepath + CONFIG_FILENAME);
  if(!f.good())
  {
    // try to read an old config file
    f.close();
    f = std::ifstream(filepath + "makegen.conf");
    oldFile = true;
  }

  // Check if the file exists
  if(f.good())
  {
    f.close();
    std::optional<ConfigFile> conf;
    if(oldFile)
      conf = ConfigFileConf::Load(realPath);
    else
      conf = ConfigFile::Load(realPath);
    if(!conf)
      return {};
    loadedConfigs.emplace(realPath, *conf);

    // Create dependency config files.
    for(size_t i = 0; i < conf->dependencies.size();++i)
    {
      std::optional<ConfigFile> dep = GetConfigFile(conf->configPath + conf->dependencies[i], loadedConfigs);
      if(dep)
      {
        conf->dependencyConfigs.push_back(*dep);
        conf->dependencies[i] = dep->configPath;
      }
      else
      {
        // Remove the dependency since it is already accounted for
        conf->dependencies.erase(conf->dependencies.begin() + i);
        --i;
      }
    }
    return conf;
  }
  return {};
}


std::optional<ConfigFile> ConfigFile::Load(const std::string& filedir)
{
  XMLObject object{XML::FromFile(filedir + CONFIG_FILENAME)};

  const std::string& target = object.GetObject("target", {XMLObject{"target", {}, "Release"}})[0].GetText();
  const std::vector<XMLObject>& configurations = object.GetObject("configuration");
  const XMLObject* configuration = nullptr;
  for(auto it = configurations.begin(); it != configurations.end(); ++it)
  {
    if(!it->HasAttribute("name"))
    {
      LOG_ERROR("No name attribute in configuration tag");
      continue;
    }
    if(it->GetAttribute("name") == target)
    {
      configuration = &(*it);
      break;
    }
  }

  if(configuration == nullptr)
  {
    LOG_ERROR("No configuration matching target: ", target); 
    return {};
  }

  ConfigFile conf;
  conf.configPath = filedir;
  conf.projectname = configuration->GetObject("projectname", 
      {XMLObject{"projectname", {}, conf.projectname}})[0].GetText();
  conf.outputname = configuration->GetObject("outputname", 
      {XMLObject{"outputname", {}, conf.outputname}})[0].GetText();
  conf.srcdir = configuration->GetObject("srcdir", 
      {XMLObject{"srcdir", {}, conf.srcdir}})[0].GetText();
  conf.outputdir = configuration->GetObject("outputdir", 
      {XMLObject{"outputdir", {}, conf.outputdir}})[0].GetText();
  conf.hFile = configuration->GetObject("hfile", 
      {XMLObject{"hfile", {}, conf.hFile}})[0].GetText();
  std::string outputtype = configuration->GetObject("outputtype", 
      {XMLObject{"outputtype", {}, "executable"}})[0].GetText();

  if(conf.srcdir[conf.srcdir.size()-1] != '/')
    conf.srcdir += '/';
  if(conf.outputdir[conf.srcdir.size()-1] != '/')
    conf.outputdir += '/';

  if(outputtype == "executable")
    conf.executable = true;
  else if(outputtype == "staticlibrary")
  {
    conf.executable = false;
    conf.shared = false;
  }
  else if(outputtype == "sharedlibrary")
  {
    conf.executable = false;
    conf.shared = true;
  }
  else
  {
    LOG_ERROR("Invalid outputtype: ", outputtype);
    LOG_ERROR("Valid arguments are executable, staticlibrary and sharedlibrary");
  }
  conf.generateHFile = configuration->GetObject("generatehfile", 
      {XMLObject{"generatehfile", {}, conf.generateHFile ? "true" : "false"}})[0].GetText() == "true";

  const int vectorCount = 6;
  std::tuple<std::vector<XMLObject>, std::vector<std::string>*, bool> vectors[vectorCount] = {
    {configuration->GetObject("library"), &conf.libs, false},
    {configuration->GetObject("libdir"), &conf.libdirs, true},
    {configuration->GetObject("includedir"), &conf.includedirs, true},
    {configuration->GetObject("define"), &conf.defines, false},
    {configuration->GetObject("compileflag"), &conf.flags, false},
    {configuration->GetObject("dependency"), &conf.dependencies, true}
  };

  for(int i = 0;i<vectorCount;++i)
  {
    const std::vector<XMLObject>& xmls = std::get<0>(vectors[i]);
    std::vector<std::string>* vec = std::get<1>(vectors[i]);
    bool isDir = std::get<2>(vectors[i]);
    for(auto it = xmls.begin(); it != xmls.end();++it)
    {
      if(it->GetText() != "")
      {
        std::string s = it->GetText();
        if(isDir && s[s.size()-1] != '/')
          s += '/';
        vec->push_back(s);
      }
    }
  }
  return conf;
}

void ConfigFile::InputBoolean(const std::string& inputText, bool& b)
{
  std::string input;
  while(true)
  {
    LOG_INFO(inputText);
    std::getline(std::cin, input);
    if(input.length() > 0)
    {
      if(input[0] == 'y' || input[0] == 'n')
      {
        b = input[0] == 'y';
        return;
      }
    }
  }
}

void ConfigFile::InputString(const std::string& inputText, std::string& str, bool needEnding, bool allowEmpty)
{
  str = "";
  while(true)
  {
    LOG_INFO(inputText);
    std::getline(std::cin, str);
    if(needEnding && str[str.length()-1] != '/' && !str.empty())
      str += '/';
    if(allowEmpty || !str.empty())
      return;
  }
}

void ConfigFile::InputMultiple(const std::string& inputText, std::vector<std::string>& vec, bool needEnding)
{
  std::string input;
  while(true)
  {
    InputString(inputText, input, needEnding, true);
    if(input == "")
      break;
    vec.push_back(input);
  }
}

ConfigFile ConfigFile::Gen()
{
  ConfigFile conf;
  InputBoolean("Should it be compiled as an executable? (y/n)", conf.executable);
  // If it isn't an executable there is not need to have libraries
  if(conf.executable)
  {
    InputMultiple("Enter library:", conf.libs,false);
    InputMultiple("Enter library directory:", conf.libdirs,true);
    InputMultiple("Enter project dependencies:", conf.dependencies,true);
  }
  else
  {
    InputBoolean("Should it be compiled as a shared library? (y/n)", conf.shared);
    InputBoolean("Should it compile a project h-file? (y/n):", conf.generateHFile);
    if(conf.generateHFile)
    {
      InputString("Enter the project h-file name (relative to source directory): ", conf.hFile, false, false);
    }
  }
  InputMultiple("Enter include directory:", conf.includedirs, true);
  InputString("Enter source directories:", conf.srcdir, true, false);
  InputMultiple("Enter preprocessor definitions:", conf.defines, false);
  InputMultiple("Enter compile flags:", conf.flags, false);
  InputString("Enter output directory (default: bin):", conf.outputdir, true, true);
  if(conf.outputdir == "")
    conf.outputdir = "bin/";
  InputString("Enter a name for the project:", conf.projectname, false, false);
  InputString("Enter a name for the output file:", conf.outputname, false, false);
  return conf;
}

void ConfigFile::Save() const
{
  XMLObject makegen("makegen", {}, std::map<std::string, std::vector<XMLObject>>{});

  // Version, target and configuration is probably going to be used in the future
  makegen.AddXMLObject(XMLObject("version", {}, "v1.3.0"));
  makegen.AddXMLObject(XMLObject("target", {}, "Release"));

  XMLObject configuration("configuration", {{"name", "Release"}}, std::map<std::string, std::vector<XMLObject>>{});
  configuration.AddXMLObject(XMLObject("projectname", {}, projectname));
  configuration.AddXMLObject(XMLObject("outputname", {}, outputname));
  configuration.AddXMLObject(XMLObject("srcdir", {}, srcdir));
  configuration.AddXMLObject(XMLObject("outputdir", {}, outputdir));
  configuration.AddXMLObject(XMLObject("hfile", {}, hFile));
  configuration.AddXMLObject(XMLObject("outputtype", {}, 
        executable ? "executable" : (shared ? "sharedlibrary" : "staticlibrary")));
  configuration.AddXMLObject(XMLObject("generatehfile", {}, generateHFile ? "true" : "false"));

  for(auto it = libs.begin();it != libs.end(); ++it)
    configuration.AddXMLObject({"library",{},*it});
  for(auto it = libdirs.begin();it != libdirs.end(); ++it)
    configuration.AddXMLObject({"libdir",{},*it});
  for(auto it = includedirs.begin();it != includedirs.end(); ++it)
    configuration.AddXMLObject({"includedir",{},*it});
  for(auto it = defines.begin();it != defines.end(); ++it)
    configuration.AddXMLObject({"define",{},*it});
  for(auto it = flags.begin();it != flags.end(); ++it)
    configuration.AddXMLObject({"compileflag",{},*it});
  for(auto it = dependencies.begin();it != dependencies.end(); ++it)
    configuration.AddXMLObject({"dependency",{},*it});

  makegen.AddXMLObject(configuration);
  std::ofstream file("makegen.xml");
  file << makegen;
}
