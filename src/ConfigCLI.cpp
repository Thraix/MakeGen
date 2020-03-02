#include "ConfigCLI.h"

#include "Common.h"
#include "ConfigFile.h"

#include <set>

void ConfigCLI::DisplayCLIHelp()
{
  LOG_INFO(1+(char*)R"(
MakeGen conf is used to create, modify and query the makegen.xml file.

Usage: makegen conf <command> [<args>] [--help]

Generating config files
  gen            Prompt the user to enter information to create config

Modifying config settings
  add            Add values to config settings which support multiple arguments
  remove         Remove values from config settings which support multiple
                  arguments
  set            Set value to config settings which support only one argument

Querying config settings
  get            Get value of the config setting)");
}

void ConfigCLI::DisplayGenHelp()
{
  LOG_INFO(1+(char*)R"(
Generate a config file from prompts

Usage: makegen conf gen <option>

options:
  prompt         Prompt the user for all needed settings
  default        Generate a default config file. Source directory is set to
                  src/, outputdir is set to bin/ and project name is set to
                  the current directory name.)");
}

void ConfigCLI::DisplayAddHelp()
{
  LOG_INFO(1+(char*)R"(
Add values to config settings which support multiple arguments

Usage: makegen conf add <setting> <value> [<values>]

Valid settings are:
  library        Library
  librarydir     Library directory
  includedir     Include directory
  define         Preprocessor define
  cflag          g++ compiler flag
  lflag          g++ linking flag
  dependency     Project which current project depends on
  excludesource  Exclude source file from compiling
  excludeheader  Exclude header file from project h-file)");
}

void ConfigCLI::DisplayRemoveHelp()
{
  LOG_INFO(1+(char*)R"(
Remove values to config settings which support multiple

Usage: makegen conf remove <setting> <value> [<

Valid settings are
  library        Library name
  librarydir     Library directory
  includedir     Include directory
  define         Preprocessor define
  cflag          g++ compiler flag
  lflag          g++ linking flag
  dependency     Project which current project depends on
  excludesource  Exclude source file from compiling
  excludeheader  Exclude header file from project h-file)");
}

void ConfigCLI::DisplaySetHelp()
{
  LOG_INFO(1+(char*)R"(
Set value to config settings which only support one argument

Usage: makegen conf set <setting> <value>

Valid string settings are:
  outputdir     Directory of the compiled output
  outputname    Name of the output executable/library
  projectname   Name of the project
  outputtype    Type of the output, valid values are executable, sharedlibrary
                 and staticlibrary
  hfile         Name of the generated project h-file

Valid boolean settings are:
  genhfile      Specifies if MakeGen should generate a project h-file

Boolean values can be set to either true/t/yes/y or false/f/no/n)");
}

void ConfigCLI::DisplayGetHelp()
{
  LOG_INFO(1+(char*)R"(
Get value of the config setting

Usage: makegen conf get <setting>

Valid settings are:
  library       Library name
  librarydir    Library directory
  includedir    Include directory
  define        Preprocessor define
  cflag         g++ compiler flag
  lflag         g++ linking flag
  dependency     Project which current project depends on
  excludesource  Exclude source file from compiling
  excludeheader  Exclude header file from project h-file
  outputdir     Directory of the compiled output
  outputname    Name of the output executable/library
  outputtype    Type of the output, valid values are executable, sharedlibrary
                 and staticlibrary
  projectname   Name of the project
  hfile         Name of the generated project h-file
  genhfile      Specifies if MakeGen should generate a project h-file)");
}


ConfigSetting ConfigCLI::CLIStringToSetting(const std::string& s)
{
  static std::map<std::string, ConfigSetting> map{
    {"srcdir", ConfigSetting::SourceDir},
    {"outputdir", ConfigSetting::OutputDir},
    {"outputname", ConfigSetting::OutputName},
    {"outputtype", ConfigSetting::OutputType},
    {"projectname", ConfigSetting::ProjectName},
    {"hfile", ConfigSetting::HFileName},
    {"library", ConfigSetting::Library},
    {"librarydir", ConfigSetting::LibraryDir},
    {"includedir", ConfigSetting::IncludeDir},
    {"define", ConfigSetting::Define},
    {"cflag", ConfigSetting::CFlag},
    {"lflag", ConfigSetting::LFlag},
    {"excludesource", ConfigSetting::ExcludeSource},
    {"excludeheader", ConfigSetting::ExcludeHeader},
    {"dependency", ConfigSetting::Dependency},
    {"genhfile", ConfigSetting::GenerateHFile},
  };
  auto it = map.find(s);
  if(it == map.end())
    return ConfigSetting::Invalid;
  return it->second;
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
  if(option == "default")
  {
    ConfigFile{FileUtils::GetRealPath("."),0}.Save();
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

  ConfigSetting setting = CLIStringToSetting(argv[1]);
  if(!ConfigUtils::IsVectorSetting(setting))
  {
    if(setting == ConfigSetting::Invalid)
    {
      LOG_ERROR("No such setting: ", argv[1]);
    }
    else
    {
      LOG_ERROR("Cannot remove setting which only supports one argument");
      LOG_ERROR("use set instead.");
    }
    return 1;
  }
  for(int i = 2; i<argc;++i)
  {
    config.AddSettingVectorString(setting, argv[i]);
  }

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

  ConfigSetting setting = CLIStringToSetting(argv[1]);
  if(!ConfigUtils::IsVectorSetting(setting))
  {
    if(setting == ConfigSetting::Invalid)
    {
      LOG_ERROR("No such setting: ", argv[1]);
    }
    else
    {
      LOG_ERROR("Cannot remove setting which only supports one argument");
      LOG_ERROR("use set instead.");
    }
    return 1;
  }

  for(int i = 2; i<argc;++i)
  {
    config.RemoveSettingVectorString(setting, argv[i]);
  }

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

  ConfigSetting setting = CLIStringToSetting(argv[1]);
  if(!ConfigUtils::IsStringSetting(setting) && !ConfigUtils::IsBoolSetting(setting))
  {
    if(setting == ConfigSetting::Invalid)
    {
      LOG_ERROR("No such setting: ", argv[1]);
    }
    else
    {
      LOG_ERROR("Cannot set setting which supports multiple arguments");
      LOG_ERROR("use add or remove instead.");
    }
    return 1;
  }

  config.SetSettingString(setting, argv[2]);
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
  ConfigSetting setting = CLIStringToSetting(argv[1]);
  std::vector<std::string> vector = config.GetSetting(setting);
  for(auto it = vector.begin(); it != vector.end(); ++it)
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
      LOG_ERROR("Config file already exist (", CONFIG_FILENAME, ")");
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
