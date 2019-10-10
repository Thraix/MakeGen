#include "CLI.h"

#include "Common.h"
#include "ConfigFile.h"

void CLI::DisplayCLIHelp()
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
  LOG_INFO("  get             List all values of the config setting");
  // --------------------------------------------------------------------------------------|
}

int CLI::Main(int argc, char** argv)
{
  // Do nothing
  if(argc < 2 || std::string(argv[1]) == "--help")
  {
    DisplayCLIHelp();
    return 0;
  }
  std::string command = argv[1];
  if(command == "gen")
  {
    ConfigFile::Gen().Save();
    return 0;
  }
  else
  {
    LOG_ERROR("Unknown config command: ", command);
    return 1;
  }
  return 0;
}
