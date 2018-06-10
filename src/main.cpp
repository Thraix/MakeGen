#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include "Common.h"
#include "IncludeDeps.h"
#include "ConfigFile.h"
#include "Makefile.h"

void GenMakefile()
{
  ConfigFile conf = ConfigFile::Load(); 
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

int main(int argc, char** argv)
{
  unsigned int flags = ReadFlags(argc,argv);
  if(flags & FLAG_HELP)
  {
    LOG_INFO("Usage: makegen [options]");
    LOG_INFO(" Options:");
    LOG_INFO("  --help\tDisplays this information");
    LOG_INFO("  --conf\tGenerate a config file for the project");
    LOG_INFO("  --version\tDisplays the version of this program");
    LOG_INFO("  install\tGenerates a Makefile and runs make install");
    LOG_INFO("  clean\t\tGenerates a Makefile and runs make clean");
    LOG_INFO("  rebuild\tGenerates a Makefile and runs make rebuild");
    LOG_INFO(" If no option is given it will generate a Makefile and run default make");
    return 0;
  }
  if(flags & FLAG_VERSION)
  {
    LOG_INFO("MakeGen ",VERSION);
    return 0;
  }
  if(flags & FLAG_GEN)
  {
    ConfigFile::Gen().Save();
    return 0;
  }
  LOG_INFO("Generating Makefile...");
  GenMakefile();
  LOG_INFO("Running Makefile...");
  for(int i = 1;i<argc;i++)
  {
    if(argv[i][0] != '-')
    {
      std::string make = std::string("make ") + argv[i];
      system(make.c_str());
      return 0;
    }
  }

  system("make");
  
  return 0;
}
