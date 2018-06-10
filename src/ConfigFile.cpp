#include "ConfigFile.h"

#include <fstream>
#include "Common.h"

#define FLAG_NONE 0
#define FLAG_VECTOR 1
#define FLAG_STRING 2
#define FLAG_BOOL 3 
ConfigFile::ConfigFile()
  : outputdir("bin"), outputname("out.a"),executable(true)
{

}

ConfigFile ConfigFile::Load()
{
  ConfigFile conf;
  unsigned int loadFlag = 0;

  std::vector<std::string>* vec;
  std::string* s;
  bool* b;

  std::ifstream file("makegen.conf");
  std::string line;

  while(std::getline(file,line))
  {
    if(line[0]=='#')
    {
      if(line == "#libs")
      {
        vec = &conf.libs;
        loadFlag = FLAG_VECTOR;
      }
      else if(line == "#libdirs")
      {
        vec = &conf.libdirs;
        loadFlag = FLAG_VECTOR;
      }
      else if(line == "#includedirs")
      {
        vec = &conf.includedirs;
        loadFlag = FLAG_VECTOR;
      }
      else if(line == "#srcdirs")
      {
        vec = &conf.srcdirs;
        loadFlag = FLAG_VECTOR;
      }
      else if(line == "#defines")
      {
        vec = &conf.defines;
        loadFlag = FLAG_VECTOR;
      }
      else if(line == "#outputdir")
      {
        s = &conf.outputdir;
        loadFlag = FLAG_STRING;
      }
      else if(line == "#outputname")
      {
        s = &conf.outputname;
        loadFlag = FLAG_STRING;
      }
      else if(line == "#projectname")
      {
        s = &conf.projectname;
        loadFlag = FLAG_STRING;
      }
      else if(line == "#executable")
      {
        b = &conf.executable;
        loadFlag = FLAG_BOOL;
      }
      else
      {
        LOG_ERROR("Invalid flag");
      }
    }
    else
    {
      if(loadFlag == FLAG_STRING)
      {
        *s = line;
      }
      else if(loadFlag == FLAG_VECTOR)
      {
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
  return conf;
}

void ConfigFile::InputMultiple(const std::string& inputText, std::vector<std::string>& vec, bool needEnding)
{
  std::string input;
  while(true) 
  {
    LOG_INFO(inputText);
    std::getline(std::cin, input);
    if(input == "")
      break;
    if(needEnding && input[input.length()-1] != '/')
      input+='/';
    vec.push_back(input);
  }
}

ConfigFile ConfigFile::Gen()
{
  ConfigFile conf;
  std::string input = "";
  while(input == "")
  {
    LOG_INFO("Should it be compiled as an executable (y/n):");
    std::getline(std::cin, input);
    if(input[0] != 'y' && input[0] != 'n')
      input = "";
  }
  conf.executable = input[0] == 'y';
  // If it isn't an executable there is not need to have libraries
  if(conf.executable)
  {
    InputMultiple("Enter library:", conf.libs,false);
    InputMultiple("Enter library directory:", conf.libdirs,true);
  }
  InputMultiple("Enter include directory:", conf.includedirs,true);
  InputMultiple("Enter source directories:", conf.srcdirs,true);
  InputMultiple("Enter preprocessor definitions:", conf.defines,false);
  LOG_INFO("Enter output directory (default: bin):");
  std::getline(std::cin, conf.outputdir);
  if(conf.outputdir == "")
    conf.outputdir = "bin/";
  conf.outputname = "";
  while(conf.projectname == "")
  {
    LOG_INFO("Enter a name for the project:");
    std::getline(std::cin, conf.projectname);
  }
  while(conf.outputname == "")
  {
    LOG_INFO("Enter a name for the output file:");
    std::getline(std::cin, conf.outputname);
  }
  return conf;
}

void ConfigFile::Save() const
{
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
  file << "#defines" << std::endl;
  for(auto it = defines.begin();it!=defines.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#outputdir" << std::endl;
  file << outputdir << std::endl;
  file << "#projectname" << std::endl;
  file << projectname << std::endl;
  file << "#outputname" << std::endl;
  file << outputname << std::endl;
  file << "#executable" << std::endl;
  file << (executable ? "true" : "false") << std::endl;
  file.close();
}
