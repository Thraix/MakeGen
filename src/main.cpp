#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include "IncludeDeps.h"
#include "ConfigFile.h"
#include "Makefile.h"
#include "Logging.h"

#define BIT(x) (1<<x)

const static unsigned int FLAG_HELP = BIT(0);
const static unsigned int FLAG_GEN= BIT(1);

// Flags for loading conf file
const static unsigned int LOAD_FLAG_ERROR= BIT(0);
const static unsigned int LOAD_FLAG_VECTOR = BIT(1);
const static unsigned int LOAD_FLAG_STRING = BIT(2);


int flags = 0;

void GenMakefile()
{
  ConfigFile conf = ConfigFile::Load(); 
  Makefile::Save(conf);
}

void ReadFlags(int argc, char** argv)
{
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
      }
    }
  }
}

void InputMultiple(const std::string& inputText, std::vector<std::string>& ret)
{
  std::string input;
  while(true) 
  {
    LOG_INFO(inputText);
    std::getline(std::cin, input);
    if(input == "")
      break;
    ret.push_back(input);
  }

}

void GenConfFile()
{
  std::vector<std::string> libs;
  std::vector<std::string> libdirs;
  std::vector<std::string> includedirs;
  std::vector<std::string> srcdirs;
  std::string outputDir;
  InputMultiple("Enter library:", libs);
  InputMultiple("Enter library directory:", libdirs);
  InputMultiple("Enter include directory:", includedirs);
  InputMultiple("Enter source directories:", srcdirs);
  LOG_INFO("Enter output directory (default: bin):");
  std::getline(std::cin, outputDir);
  if(outputDir == "")
    outputDir = "bin";

  std::ofstream file("makegen.conf");
  file << "#libs" << std::endl;
  for(auto it = libs.begin();it!=libs.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#libdirs" << std::endl;
  for(auto it = libdirs.begin();it!=libdirs.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#includedirs" << std::endl;
  for(auto it = includedirs.begin();it!=includedirs.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#srcdirs" << std::endl;
  for(auto it = srcdirs.begin();it!=srcdirs.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#outputdir" << std::endl;
  file << outputDir << std::endl;
  file.close();
}

int main(int argc, char** argv)
{
  ReadFlags(argc,argv);
  if((flags & FLAG_HELP))
  {
    LOG_INFO("Usage: makegen [options]");
    LOG_INFO(" Options:");
    LOG_INFO("  --help\tDisplays this information");
    LOG_INFO("  --conf\tGenerate a config file for the project");
    LOG_INFO("  install\tGenerates a Makefile and runs make install");
    LOG_INFO("  clean\tGenerates a Makefile and runs make clean");
    LOG_INFO("  rebuild\tGenerates a Makefile and runs make rebuild");
    LOG_INFO(" If no option is given it will generate a Makefile and run default make");
    return 0;
  }
  if(flags & FLAG_GEN)
  {
    GenConfFile();
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
