#include "ConfigFile.h"

#include "FileUtils.h"
#include "compatibility/ConfigFileConf.h"
#include "xml/XML.h"

#include <fstream>

ConfigFile::ConfigFile(const std::string& path, const FlagData& flagData, int)
  : configPath{path}
{
  // Converts project name (current directory) to lowercase
  // and replace whitespace with underscore
  // Create xml
  XMLObject makegen("makegen", {}, std::map<std::string, std::vector<XMLObject>>{});

  // Version, target and configuration is probably going to be used in the future
  makegen.AddXMLObject(XMLObject("version", {}, "v1.3.0"));
  if(flagData.flags & FLAG_TARGET)
    makegen.AddXMLObject(XMLObject("target", {}, flagData.target));
  else
    makegen.AddXMLObject(XMLObject("target", {}, "Release"));

  XMLObject configuration("configuration", {{"name", "Release"}}, std::map<std::string, std::vector<XMLObject>>{});
  configuration.AddXMLObject(XMLObject("projectname", {}, ConfigUtils::GetDefaultProjectName(configPath)));
  configuration.AddXMLObject(XMLObject("outputname", {}, ConfigUtils::GetDefaultOutputName(configPath)));
  configuration.AddXMLObject(XMLObject("srcdir", {}, "src/"));
  configuration.AddXMLObject(XMLObject("outputdir", {}, "bin/"));
  configuration.AddXMLObject(XMLObject("hfilename", {}, ConfigUtils::GetDefaultHFileName(configPath)));
  configuration.AddXMLObject(XMLObject("outputtype", {}, "executable"));
  configuration.AddXMLObject(XMLObject("generatehfile", {}, "false"));

  makegen.AddXMLObject(configuration);
  config = makegen;
  Init(flagData);
}

ConfigFile::ConfigFile(const std::string& path, const FlagData& flagData)
  : config{XML::FromFile(path + CONFIG_FILENAME)}, configPath{path}
{
  Init(flagData);
}

ConfigFile::ConfigFile(XMLObject& config, const std::string& path, const FlagData& flagData)
  : config{config}, configPath{path}
{
  Init(flagData);
}

void ConfigFile::Init(const FlagData& flagData)
{
  if (flagData.flags & FLAG_TARGET)
  {
    target = flagData.target;
  }
  else
  {
    const std::vector<XMLObject>* targetXml = config.GetObjectPtr("target");
    target = "Release";
    if(!targetXml || targetXml->size() == 0)
    {
      LOG_ERROR("No target found in config file. Using target=", target);
      return;
    }
    if(targetXml->size() > 1)
      LOG_ERROR("To many targets in config file. Using target=", (*targetXml)[0].GetText());
    if(targetXml->size() > 0)
      target = (*targetXml)[0].GetText();
  }

}

std::string& ConfigFile::GetSettingString(ConfigSetting setting)
{
  // Adding it to the cache since we need to return a reference
  if(!ConfigUtils::IsStringSetting(setting))
  {
    LOG_ERROR("Invalid string setting");
    return cache.strings.emplace("invalid", "").first->second;
  }

  std::string sSetting = ConfigUtils::GetSettingName(setting);
  auto it = cache.strings.find(sSetting);
  if(it != cache.strings.end())
    return it->second;

  const std::vector<XMLObject>* values = GetConfiguration().GetObjectPtr(sSetting);

  // No value found, using default
  if(values == nullptr)
    return cache.strings.emplace(sSetting, ConfigUtils::GetDefaultSettingString(setting, configPath)).first->second;

  if(values->size() != 1)
  {
    LOG_ERROR("To many arguments for setting using first: ", (int)setting, "=", (*values)[0].GetText());
  }
  std::string s = (*values)[0].GetText();
  if(ConfigUtils::IsDirectory(setting) && !s.empty() && s[s.size()-1] != '/')
    s += '/';
  return cache.strings.emplace(sSetting, s).first->second;
}

bool ConfigFile::GetSettingBool(ConfigSetting setting)
{
  if(setting == ConfigSetting::Invalid)
  {
    LOG_ERROR("Invalid config setting");
    return false;
  }
  std::string sSetting = ConfigUtils::GetSettingName(setting);
  auto it = cache.bools.find(sSetting);
  if(it != cache.bools.end())
    return it->second;

  const std::vector<XMLObject>* values = GetConfiguration().GetObjectPtr(sSetting);//, 

  if(values == nullptr)
    return cache.bools.emplace(sSetting, ConfigUtils::GetDefaultSettingBool(setting)).first->second;

  if(values->size() != 1)
  {
    LOG_ERROR("To many arguments for setting using first: ", (int)setting, "=", (*values)[0].GetText());
  }
  return cache.bools.emplace(sSetting, (*values)[0].GetText() == "true").first->second;
}

std::vector<std::string>& ConfigFile::GetSettingVectorString(ConfigSetting setting)
{
  std::string sSetting = ConfigUtils::GetSettingName(setting);
  auto it = cache.vecStrings.find(sSetting);
  if(it != cache.vecStrings.end())
    return it->second;

  const std::vector<XMLObject>* values = GetConfiguration().GetObjectPtr(sSetting);
  if(values == nullptr)
    return cache.vecStrings.emplace(sSetting, std::vector<std::string>{}).first->second;

  std::vector<std::string> strings;
  strings.reserve(values->size());
  for(auto it = values->begin(); it != values->end(); ++it)
  {
    if(it->GetText() == "")
      continue;
    std::string s = it->GetText();
    if(ConfigUtils::IsDirectory(setting) && s[s.size()-1] != '/')
      s += '/';
    strings.push_back(s);
  }

  return cache.vecStrings.emplace(sSetting, strings).first->second;
}

std::vector<std::string> ConfigFile::GetSetting(ConfigSetting setting)
{
  if(ConfigUtils::IsStringSetting(setting))
    return {GetSettingString(setting)};
  else if(ConfigUtils::IsVectorSetting(setting))
    return GetSettingVectorString(setting);
  else if(ConfigUtils::IsBoolSetting(setting))
    return {GetSettingBool(setting) ? "true" : "false"};
  else
  {
    LOG_ERROR("Invalid config setting");
    return {};
  }
}

bool ConfigFile::SetSettingString(ConfigSetting setting, const std::string& value)
{
  // Check if valid enum
  std::string s = value;
  std::string sSetting = ConfigUtils::GetSettingName(setting);
  if(ConfigUtils::IsStringSetting(setting))
  {
    if(ConfigUtils::IsDirectory(setting) && s[s.size()-1] != '/')
    {
      s += '/';
    }
    auto it = cache.strings.find(sSetting);
    // Update cache
    if(it != cache.strings.end())
      it->second = s;
    else
      cache.strings.emplace(sSetting, s);
  }
  else if(ConfigUtils::IsBoolSetting(setting))
  {
    if(s == "true" || s == "t" || s == "yes" || s == "y")
      s = "true";
    else if(s == "false" || s == "f" || s == "no" || s == "n")
      s = "false";
    else
    {
      LOG_ERROR("Invalid boolean value: ", s);
      return false;
    }

    auto it = cache.bools.find(sSetting);
    // Update cache
    if(it != cache.bools.end())
      it->second = s == "true";
    else
      cache.bools.emplace(sSetting, value == "true");
  }
  else
  {
    LOG_ERROR("Not a string setting");
    return false;
  }

  XMLObject& configuration = GetConfiguration();
  std::vector<XMLObject>* values = configuration.GetObjectPtr(sSetting);
  if(values == nullptr)
    configuration.AddXMLObject({sSetting, {}, s});
  else if(values->size() > 1)
    LOG_ERROR("Multiple values of setting, changing first: ", sSetting, "=", s);
  else
    (*values)[0].SetText(s);
  return true;
}

bool ConfigFile::AddSettingVectorString(ConfigSetting setting, const std::string& value)
{
  // Check if valid enum
  if(ConfigUtils::IsVectorSetting(setting))
  {
    std::string s = value;
    if(ConfigUtils::IsDirectory(setting) && s[s.size()-1] != '/')
    {
      s += '/';
    }
    std::string sSetting = ConfigUtils::GetSettingName(setting);
    auto it = cache.vecStrings.find(sSetting);

    // Update cache
    if(it != cache.vecStrings.end())
      it->second.push_back(s);
    else
      cache.vecStrings.emplace(sSetting, std::vector<std::string>{s});

    GetConfiguration().AddXMLObject({sSetting, {}, s});
  }
  else
  {
    LOG_ERROR("Not a vector setting");
    return false;
  }
  return true;
}

bool ConfigFile::RemoveSettingVectorString(ConfigSetting setting, const std::string& value)
{
  // Check if valid enum
  if(ConfigUtils::IsVectorSetting(setting))
  {
    std::string s = value;
    if(ConfigUtils::IsDirectory(setting) && s[s.size()-1] != '/')
    {
      s += '/';
    }
    std::string sSetting = ConfigUtils::GetSettingName(setting);

    auto it = cache.vecStrings.find(sSetting);
    if(it != cache.vecStrings.end())
    {
      // Update cache
      for(auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
      {
        if(*itVec == s)
        {
          it->second.erase(itVec);
        }
      }
    }

    std::vector<XMLObject>* values = GetConfiguration().GetObjectPtr(sSetting);
    bool found = false;
    for(auto it = values->begin(); it != values->end();++it)
    {
      if(it->GetText() == s)
      {
        values->erase(it);
        found = true;
        break;
      }
    }
    if(!found)
    {
      LOG_ERROR("Couldn't find value: ", s);
      return false;
    }
  }
  else
  {
    LOG_ERROR("Not a vector setting");
    return false;
  }
  return false;
}

XMLObject& ConfigFile::GetConfiguration()
{
  std::vector<XMLObject>* configurations = config.GetObjectPtr("configuration");
  if(configurations == nullptr || configurations->size() == 0)
  {
    LOG_ERROR("No configuration in makegen.xml");
    assert(false);
  }
  for(auto it = configurations->begin(); it != configurations->end(); ++it)
  {
    if(!it->HasAttribute("name"))
    {
      LOG_ERROR("No name attribute in configuration tag");
      continue;
    }
    if(it->GetAttribute("name") == target)
    {
      return *it;
    }
  }

  LOG_ERROR("Couldn\'t find given target in config file. Using target=", (*configurations)[0].HasAttribute("name") ? (*configurations)[0].GetAttribute("name") : "");
  return (*configurations)[0];
}

const std::string& ConfigFile::GetConfigPath() const
{
  return configPath;
}

ConfigFile& ConfigFile::GetDependencyConfig(size_t i)
{
  return dependencyConfigs[i];
}

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath, const FlagData& flagData)
{
  std::map<std::string, ConfigFile> loadedConfigs;
  return GetConfigFile(filepath, loadedConfigs, flagData);
}

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath, std::map<std::string, ConfigFile>& loadedConfigs, const FlagData& flagData)
{
  std::string realPath = FileUtils::GetRealPath(filepath);
  if(realPath == "")
    return {};
  auto it = loadedConfigs.find(realPath);
  if(it != loadedConfigs.end())
  {
    return {};
  }

  bool oldFile = false;
  std::ifstream f(filepath + CONFIG_FILENAME);
  if(!f.good())
  {
    ConfigFileConf::CreateXMLFile(realPath);
    // try to read an old config file
    f.close();
    f = std::ifstream(filepath + CONFIG_FILENAME);
  }

  // Check if the file exists
  if(f.good())
  {
    f.close();
    ConfigFile conf = ConfigFile(filepath, flagData);
    if(conf.hasInitError)
      return {};
    loadedConfigs.emplace(realPath, conf);

    std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
    // Create dependency config files.
    for(size_t i = 0; i < dependencies.size();++i)
    {
      std::optional<ConfigFile> dep = GetConfigFile(conf.configPath + dependencies[i], loadedConfigs, flagData);
      if(dep)
      {
        conf.dependencyConfigs.push_back(*dep);
        dependencies[i] = dep->configPath;
      }
      else
      {
        // Remove the dependency since it is already accounted for
        dependencies.erase(dependencies.begin() + i);
        --i;
      }
    }
    return conf;
  }
  return {};
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

ConfigFile ConfigFile::Gen(const FlagData& flagData)
{
  bool executable, shared, generateHFile;
  std::vector<std::string> libs, libdirs, includedirs, defines, compileFlags, linkingFlags, dependencies, excludeSources, excludeHeaders;
  std::string srcdir, outputdir, projectname, outputname, hFile;

  InputBoolean("Should it be compiled as an executable? (y/n)", executable);
  // If it isn't an executable there is not need to have libraries
  if(executable)
  {
    InputMultiple("Enter library:", libs,false);
    InputMultiple("Enter library directory:", libdirs,true);
    InputMultiple("Enter project dependencies:", dependencies,true);
  }
  else
  {
    InputBoolean("Should it be compiled as a shared library? (y/n)", shared);
    InputBoolean("Should it compile a project h-file? (y/n):", generateHFile);
    if(generateHFile)
    {
      InputString("Enter the project h-file name (relative to source directory): ", hFile, false, false);
    }
  }
  InputMultiple("Enter include directory:", includedirs, true);
  InputString("Enter source directories:", srcdir, true, false);
  InputMultiple("Enter preprocessor definitions:", defines, false);
  InputMultiple("Enter compile flags:", compileFlags, false);
  InputMultiple("Enter linking flags:", linkingFlags, false);
  InputMultiple("Enter excluded source files flags:", excludeSources, false);
  InputMultiple("Enter excluded header files flags:", excludeHeaders, false);
  InputString("Enter output directory (default: bin):", outputdir, true, true);
  if(outputdir == "")
    outputdir = "bin/";
  InputString("Enter a name for the project:", projectname, false, false);
  InputString("Enter a name for the output file:", outputname, false, false);


  // Create xml
  XMLObject makegen("makegen", {}, std::map<std::string, std::vector<XMLObject>>{});

  // Version, target and configuration is probably going to be used in the future
  makegen.AddXMLObject(XMLObject("version", {}, "v1.3.0"));
  if(flagData.flags & FLAG_TARGET)
    makegen.AddXMLObject(XMLObject("target", {}, flagData.target));
  else
    makegen.AddXMLObject(XMLObject("target", {}, "Release"));

  XMLObject configuration("configuration", {{"name", "Release"}}, std::map<std::string, std::vector<XMLObject>>{});
  configuration.AddXMLObject(XMLObject("projectname", {}, projectname));
  configuration.AddXMLObject(XMLObject("outputname", {}, outputname));
  configuration.AddXMLObject(XMLObject("srcdir", {}, srcdir));
  configuration.AddXMLObject(XMLObject("outputdir", {}, outputdir));
  configuration.AddXMLObject(XMLObject("hfilename", {}, hFile));
  configuration.AddXMLObject(XMLObject("outputtype", {},
        executable ? "executable" : (shared ? "sharedlibrary" : "staticlibrary")));
  configuration.AddXMLObject(XMLObject("generatehfile", {}, generateHFile ? "true" : "false"));

  for(auto it = libs.begin();it != libs.end(); ++it)
    configuration.AddXMLObject({"library",{},*it});
  for(auto it = libdirs.begin();it != libdirs.end(); ++it)
    configuration.AddXMLObject({"librarydir",{},*it});
  for(auto it = includedirs.begin();it != includedirs.end(); ++it)
    configuration.AddXMLObject({"includedir",{},*it});
  for(auto it = defines.begin();it != defines.end(); ++it)
    configuration.AddXMLObject({"define",{},*it});
  for(auto it = compileFlags.begin();it != compileFlags.end(); ++it)
    configuration.AddXMLObject({"cflag",{},*it});
  for(auto it = dependencies.begin();it != dependencies.end(); ++it)
    configuration.AddXMLObject({"dependency",{},*it});

  makegen.AddXMLObject(configuration);
  return ConfigFile{makegen, FileUtils::GetRealPath("."), flagData};
}

void ConfigFile::Save() const
{
  std::ofstream file("makegen.xml");
  file << config;
}
