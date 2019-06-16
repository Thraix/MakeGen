#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include "Common.h"
#include "IncludeDeps.h"
#include "ConfigFile.h"
#include "Makefile.h"
#include "HFileGen.h"
#include "Timer.h"

void PrintHelp()
{
  LOG_INFO("Usage: makegen [options]");
  LOG_INFO(" Options:");
  LOG_INFO("  --help\tDisplays this information");
  LOG_INFO("  --conf\tGenerate a config file for the project");
  LOG_INFO("  --version\tDisplays the version of this program");
  LOG_INFO("  install\tGenerates a Makefile and runs make install");
  LOG_INFO("  clean\t\tGenerates a Makefile and runs make clean");
  LOG_INFO("  rebuild\tGenerates a Makefile and runs make rebuild");
  LOG_INFO(" If no option is given it will run default make");
}

std::optional<ConfigFile> GetConfigFile(const std::string& filepath)
{
  std::ifstream f(filepath + CONFIG_FILENAME);
  if(f.good())
  {
    ConfigFile conf = ConfigFile::Load(filepath); 
    return conf;
  }
  f.close();
  return {};
}

void GenMakefile(const ConfigFile& conf)
{
  if(conf.generateHFile)
    HFileGen::Create(conf);
  Makefile::Save(conf);
}

unsigned int ReadFlags(int argc, char** argv)
{
  unsigned int flags = 0;
  for(int i = 1;i<argc;i++)
  {
    if(strlen(argv[i]) > 1)
    {
      if(argv[i][0] == '-' && argv[i][1] == '-')
      {
        std::string flag(argv[i]);
        if(flag == "--help")
        {
          flags |= FLAG_HELP;
        }
        else if(flag == "--conf")
        {
          flags |= FLAG_GEN;
        }
        else if(flag == "--version")
        {
          flags |= FLAG_VERSION;
        }
      }
    }
  }
  return flags;
}

bool MakeGen(const std::string& filepath, const std::string& args, const ConfigFile& conf)
{
  for(size_t i = 0;i<conf.dependencies.size();++i)
  {
    bool success = MakeGen(conf.dependencies[i], args, conf.dependencyConfigs[i]);
    if(!success)
      return success;
  }
  LOG_INFO("---------------------------");
  LOG_INFO("Building ", conf.projectname);
  LOG_INFO("Generating Makefile...");
  Timer timer;
  GenMakefile(conf);
  LOG_INFO("Took ", round(timer.Elapsed()*1000.0)/1000.0,"s");
  LOG_INFO("Running Makefile...");

  return system(std::string("make --no-print-directory -C " + filepath + " " + args).c_str()) == 0;
}

int main(int argc, char** argv)
{
  unsigned int flags = ReadFlags(argc,argv);
  if(flags & FLAG_HELP)
  {
    PrintHelp();
    return 0;
  }
  if(flags & FLAG_VERSION)
  {
    LOG_INFO("MakeGen ",MAKEGEN_VERSION);
    return 0;
  }
  if(flags & FLAG_GEN)
  {
    ConfigFile::Gen().Save();
    return 0;
  }

  std::string args = std::string("");
  for(int i = 1;i<argc;i++)
  {
    args += " " + std::string(argv[i]);
  }
  auto conf = GetConfigFile("./");
  if(conf)
  {
    bool success = MakeGen("./", args, *conf);
    return success ? 0 : 1;
  }
  else
  {
    LOG_ERROR("No ", CONFIG_FILENAME, " or Makefile found.");
    PrintHelp();
  }
}
