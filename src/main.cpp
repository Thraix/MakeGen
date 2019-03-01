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

bool GenMakefile()
{
  std::ifstream f("makegen.conf");
  if(f.good())
  {
    ConfigFile conf = ConfigFile::Load("makegen.conf"); 
    if(conf.generateHFile)
      HFileGen::Create(conf);
    Makefile::Save(conf);
    return true;
  }
  f.close();
  f = std::ifstream("Makefile");

  if(!f.good())
  {
    LOG_ERROR("No makegen.conf or Makefile found.");
    PrintHelp();
    return false;
  }
  return true;
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
  LOG_INFO("Generating Makefile...");
  Timer timer;
  if(!GenMakefile())
    return 1;
  LOG_INFO("Took ", round(timer.Elapsed()*1000.0)/1000.0,"s");
  LOG_INFO("Running Makefile...");
  for(int i = 1;i<argc;i++)
  {
    if(argv[i][0] != '-')
    {
      std::string make = std::string("make ") + argv[i];
      return system(make.c_str()) == 0 ? 0 : 1;
    }
  }

  ;
  return system("make") == 0 ? 0 : 1;
}
