#include "ConfigFile.h"

#include <fstream>
#include "Logging.h"

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
