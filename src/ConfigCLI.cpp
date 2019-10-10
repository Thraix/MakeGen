#include "ConfigCLI.h"

#include "Common.h"
#include "ConfigFile.h"

#include <set>

void ConfigCLI::DisplayCLIHelp()
{
  LOG_INFO("MakeGen conf is used to create, modify and query the makegen.conf file.");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf <command> [<args>] [--help] ");
  LOG_INFO("");
  LOG_INFO("Generating config files");
  LOG_INFO("  gen             Prompt the user to enter information to create config");
  LOG_INFO("");

  LOG_INFO("Modifying config settings");
  LOG_INFO("  add             Add values to config settings which support multiple arguments");
  LOG_INFO("  remove          Remove values to config settings which support multiple");
  LOG_INFO("                   arguments");
  LOG_INFO("  set             Set value to config settings which support only one argument");
  LOG_INFO("");

  LOG_INFO("Querying config settings");
  LOG_INFO("  get             Get value of the config setting");
}

void ConfigCLI::DisplayGenHelp()
{
  LOG_INFO("Generate a config file from prompts");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf gen <option>");
  LOG_INFO("");
  LOG_INFO("options:");
  LOG_INFO("  prompt           Prompt the user for all needed settings");
}

void ConfigCLI::DisplayAddHelp()
{
  LOG_INFO("Add values to config settings which support multiple arguments");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf add <setting> <value> [<values>]");
  LOG_INFO("");
  LOG_INFO("Valid settings are:");
  LOG_INFO("  library       Library");
  LOG_INFO("  libdir        Library directory");
  LOG_INFO("  includedir    Include directory");
  LOG_INFO("  define        Preprocessor define");
  LOG_INFO("  cflag         g++ compiler flags");
  LOG_INFO("  dependency    Project which current project depends on");
}

void ConfigCLI::DisplayRemoveHelp()
{
  LOG_INFO("Remove values to config settings which support multiple arguments");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf remove <setting> <value> [<values>]");
  LOG_INFO("");
  LOG_INFO("Valid settings are:");
  LOG_INFO("  library       Library name");
  LOG_INFO("  libdir        Library directory");
  LOG_INFO("  includedir    Include directory");
  LOG_INFO("  define        Preprocessor define");
  LOG_INFO("  cflag         g++ compiler flags");
  LOG_INFO("  dependency    Project which current project depends on");
}

void ConfigCLI::DisplaySetHelp()
{
  LOG_INFO("Set value to config settings which only support one argument");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf set <setting> <value>");
  LOG_INFO("");
  LOG_INFO("Valid string settings are:");
  LOG_INFO("  outputdir     Directory of the compiled output");
  LOG_INFO("  output        Name of the output executable/library");
  LOG_INFO("  name          Name of the project");
  LOG_INFO("  hfile         Name of the generated project h-file");
  LOG_INFO("");
  LOG_INFO("Valid boolean settings are:");
  LOG_INFO("  executable    Specifies if the project be compiled as executable or library");
  LOG_INFO("  shared        Specifies if the library should be compiled as shared.");
  LOG_INFO("  genhfile      Specifies if MakeGen should generate a project h-file");
  LOG_INFO("");
  LOG_INFO("Boolean values can be set to either true/t/yes/y or false/f/no/n");
}

void ConfigCLI::DisplayGetHelp()
{
  LOG_INFO("Get value of the config setting");
  LOG_INFO("");
  LOG_INFO("Usage: makegen conf get <setting>");
  LOG_INFO("");
  LOG_INFO("Valid settings are:");
  LOG_INFO("  library       Library name");
  LOG_INFO("  libdir        Library directory");
  LOG_INFO("  includedir    Include directory");
  LOG_INFO("  define        Preprocessor define");
  LOG_INFO("  cflag         g++ compiler flags");
  LOG_INFO("  dependency    Project which current project depends on");
  LOG_INFO("  outputdir     Directory of the compiled output");
  LOG_INFO("  output        Name of the output executable/library");
  LOG_INFO("  name          Name of the project");
  LOG_INFO("  hfile         Name of the generated project h-file");
  LOG_INFO("  executable    Specifies if the project be compiled as executable or library");
  LOG_INFO("  shared        Specifies if the library should be compiled as shared.");
  LOG_INFO("  genhfile      Specifies if MakeGen should generate a project h-file");
}

std::map<std::string, std::vector<std::string>*> ConfigCLI::GetSettingVectorMap(ConfigFile& config)
{
  return {
    {"library",&config.libs},
      {"libdir",&config.libdirs},
      {"includedir",&config.includedirs},
      {"define",&config.defines},
      {"cflag",&config.flags},
      {"dependency",&config.dependencies}
  };
}


std::map<std::string, std::string*> ConfigCLI::GetSettingStringMap(ConfigFile& config) 
{
  return {
    {"outputdir", &config.outputdir},
      {"output", &config.outputname},
      {"name", &config.projectname},
      {"hfile", &config.hFile},
  };
}

std::map<std::string, bool*> ConfigCLI::GetSettingBoolMap(ConfigFile& config) 
{
  return {
    {"executable", &config.executable},
      {"shared", &config.shared},
      {"genhfile", &config.generateHFile}
  };
}

int ConfigCLI::Gen(int argc, char** argv)
{
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayGenHelp();
    return 0;
  }
  if(argc < 2)
  {
    LOG_ERROR("gen needs exactly one parameter");
    return 1;
  }
  std::string option = argv[1];
  if(option == "prompt")
  {
    ConfigFile::Gen().Save();
    return 0;
  }
  else
  {
    LOG_ERROR("Invalid option: ", option);
    return 1;
  }
}

int ConfigCLI::Add(int argc, char** argv, ConfigFile& config)
{
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayAddHelp();
    return 0;
  }
  if(argc < 3)
  {
    LOG_ERROR("add needs at least two parameters");
    return 1;
  }

  auto settingMap = GetSettingVectorMap(config);
  auto it = settingMap.find(argv[1]);
  if(it == settingMap.end())
  {
    LOG_ERROR("Invalid setting: ", argv[1]);
    return 1;
  }

  std::vector<std::string>* setting = it->second;
  std::set<std::string> settingSet{setting->begin(), setting->end()};
  for(int i = 2; i<argc;++i)
  {
    auto res = settingSet.emplace(argv[i]);
    if(!res.second)
    {
      LOG_ERROR("Duplicate value: ", argv[i]);
    }
  }
  *setting = {settingSet.begin(), settingSet.end()};

  config.Save();
  return 0;
}

int ConfigCLI::Remove(int argc, char** argv, ConfigFile& config)
{
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayRemoveHelp();
    return 0;
  }
  if(argc < 3)
  {
    LOG_ERROR("remove needs at least two parameters");
    return 1;
  }

  auto settingMap = GetSettingVectorMap(config);
  auto it = settingMap.find(argv[1]);
  if(it == settingMap.end())
  {
    LOG_ERROR("Invalid setting: ", argv[1]);
    return 1;
  }

  std::vector<std::string>* setting = it->second;
  std::set<std::string> settingSet{setting->begin(), setting->end()};
  for(int i = 2; i<argc;++i)
  {
    auto it = settingSet.find(argv[i]);
    if(it == settingSet.end())
    {
      LOG_ERROR("No such value in setting: ", argv[i]);
    }
    else
      settingSet.erase(it);
  }
  *setting = {settingSet.begin(), settingSet.end()};

  config.Save();
  return 0;
}

int ConfigCLI::Set(int argc, char** argv, ConfigFile& config)
{
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplaySetHelp();
    return 0;
  }
  if(argc != 3)
  {
    LOG_ERROR("set needs exactly two parameters");
    return 1;
  }

  auto settingStringMap = GetSettingStringMap(config);
  auto it1 = settingStringMap.find(argv[1]);
  if(it1 == settingStringMap.end())
  {
    auto settingBoolMap = GetSettingBoolMap(config);
    auto it2 = settingBoolMap.find(argv[1]);
    if(it2 == settingBoolMap.end())
    {
      LOG_ERROR("Invalid setting: ", argv[1]);
      return 1;
    }
    std::string b = argv[2];
    if(b == "true" || b == "t" || b == "yes" || b == "y")
      *(it2->second) = true;
    else if(b == "false" || b == "f" || b == "no" || b == "n")
      *it2->second = false;
    else
    {
      LOG_ERROR("Invalid boolean value: ", argv[2]);
      return 1;
    }
    config.Save();
    return 0;
  }
  *it1->second = argv[2];
  config.Save();
  return 0;
}

int ConfigCLI::Get(int argc, char** argv, ConfigFile& config)
{
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayGetHelp();
    return 0;
  }
  if(argc != 2)
  {
    LOG_ERROR("get needs exactly one parameter");
    return 1;
  }
  auto settingVectorMap = GetSettingVectorMap(config);
  auto itV = settingVectorMap.find(argv[1]);
  if(itV == settingVectorMap.end())
  {
    auto settingStringMap = GetSettingStringMap(config);
    auto itS = settingStringMap.find(argv[1]);
    if(itS == settingStringMap.end())
    {
      auto settingBoolMap = GetSettingBoolMap(config);
      auto itB = settingBoolMap.find(argv[1]);
      if(itB == settingBoolMap.end())
      {
        LOG_ERROR("Invalid setting: ", argv[1]);
        return 1;
      }
      bool* t = itB->second;
      LOG_INFO(*itB->second ? "true" : "false");
      return 0;
    }
    LOG_INFO(*itS->second);
    return 0;
  }
  for(auto it = itV->second->begin(); it != itV->second->end(); ++it)
  {
    LOG_INFO(*it);
  }
  return 0;
}

int ConfigCLI::Main(int argc, char** argv)
{
  // Do nothing
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayCLIHelp();
    return 0;
  }
  std::optional<ConfigFile> config = ConfigFile::GetConfigFile();
  std::string command = argv[1];
  if(command == "gen")
  {
    if(config)
    {
      LOG_ERROR("Config file already exist (makegen.conf)");
      return 1;
    }
    return Gen(argc-1, &argv[1]);
  }
  else if(config)
  {
    if(command == "add")
      return Add(argc-1, &argv[1], *config);
    else if(command == "remove")
      return Remove(argc-1, &argv[1], *config);
    else if(command == "set")
      return Set(argc-1, &argv[1], *config);
    else if(command == "get")
      return Get(argc-1, &argv[1], *config);
    else
    {
      LOG_ERROR("Unknown config command: ", command);
      return 1;
    }
  }
  else 
  {
    LOG_ERROR("There is no config file in the current directory");
    return 1;
  }
}
