#include "ConfigFile.h"

#include <fstream>
#include "Common.h"

#define FLAG_NONE 0
#define FLAG_VECTOR 1
#define FLAG_STRING 2
#define FLAG_BOOL 3 
ConfigFile::ConfigFile()
  : outputdir("bin"), outputname("out.a"), hFile(""),executable(true), shared(true), generateHFile(false)
{
}

ConfigFile ConfigFile::Load(const std::string& filename)
{
  ConfigFile conf;
  unsigned int loadFlag = 0;

  std::vector<std::string>* vec;
  std::string* s;
  bool* b;

  std::ifstream file(filename);
  std::string line;

  if(file.is_open())
  {
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
        else if(line == "#compileflags")
        {
          vec = &conf.flags;
          loadFlag = FLAG_VECTOR;
        }
        else if(line == "#defines")
        {
          vec = &conf.defines;
          loadFlag = FLAG_VECTOR;
        }
        else if(line == "#srcdir")
        {
          s = &conf.srcdir;
          loadFlag = FLAG_STRING;
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
        else if(line == "#hfile")
        {
          s = &conf.hFile;
          loadFlag = FLAG_STRING;
        }
        else if(line == "#executable")
        {
          b = &conf.executable;
          loadFlag = FLAG_BOOL;
        }
        else if(line == "#shared")
        {
          b = &conf.shared;
          loadFlag = FLAG_BOOL;
        }
        else if(line == "#generatehfile")
        {
          b = &conf.generateHFile;
          loadFlag = FLAG_BOOL;
        }
        else
        {
          LOG_ERROR("Invalid flag: ", line);
          loadFlag = FLAG_NONE;
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
  }
  if(conf.hFile == "")
    conf.hFile = conf.projectname+".h";

  return conf;
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

ConfigFile ConfigFile::Gen()
{
  ConfigFile conf;
  InputBoolean("Should it be compiled as an executable? (y/n)", conf.executable);
  // If it isn't an executable there is not need to have libraries
  if(conf.executable)
  {
    InputMultiple("Enter library:", conf.libs,false);
    InputMultiple("Enter library directory:", conf.libdirs,true);
  }
  else
  {
    InputBoolean("Should it be compiled as a shared library? (y/n)", conf.shared);
    InputBoolean("Should it compile a single h file? (y/n):", conf.generateHFile);
    if(conf.generateHFile)
    {
      InputString("Enter the h file name (and path): ", conf.hFile, false, false);
    }
  }
  InputMultiple("Enter include directory:", conf.includedirs, true);
  InputString("Enter source directories:", conf.srcdir, true, false);
  InputMultiple("Enter preprocessor definitions:", conf.defines, false);
  InputMultiple("Enter compile flags:", conf.flags, false);
  InputString("Enter output directory (default: bin):", conf.outputdir, true, true);
  if(conf.outputdir == "")
    conf.outputdir = "bin/";
  InputString("Enter a name for the project:", conf.projectname, false, false);
  InputString("Enter a name for the output file:", conf.outputname, false, false);
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
  file << "#defines" << std::endl;
  for(auto it = defines.begin();it!=defines.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#compileflags" << std::endl;
  for(auto it = flags.begin();it!=flags.end();++it)
  {
    file << *it << std::endl;
  }
  file << "#srcdir" << std::endl;
  file << srcdir << std::endl;
  file << "#outputdir" << std::endl;
  file << outputdir << std::endl;
  file << "#projectname" << std::endl;
  file << projectname << std::endl;
  file << "#outputname" << std::endl;
  file << outputname << std::endl;
  file << "#executable" << std::endl;
  file << (executable ? "true" : "false") << std::endl;
  file << "#generatehfile" << std::endl;
  file << (generateHFile ? "true" : "false") << std::endl;
  if(generateHFile)
  {
    file << "#hfile" << std::endl;
    file << hFile << std::endl;
  }
  if(!executable)
  {
    file << "#shared" << std::endl;
    file << (shared ? "true" : "false") << std::endl;
  }
  file.close();
}
