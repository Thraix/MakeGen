#include "ConfigFile.h"

#include "FileUtils.h"

#include <algorithm>
#include <fstream>

#define FLAG_NONE 0
#define FLAG_VECTOR 1
#define FLAG_STRING 2
#define FLAG_BOOL 3

ConfigFile::ConfigFile()
  : outputdir("bin/"), srcdir("src/"), outputname(""), projectname(FileUtils::GetCurrentDirectory()), hFile(""), executable(true), shared(true), generateHFile(false)
{
  // Converts project name (current directory) to lowercase
  // and replace whitespace with underscore
  std::transform(
      projectname.begin(),
      projectname.end(),
      std::back_inserter(outputname),
      [](unsigned char c)
      {
        if(c == ' ')
          return '_';
        return (char)std::tolower(c);
      });

  // Removes all other characters
  auto it = std::remove_if(
      outputname.begin(),
      outputname.end(),
      [](unsigned char c)
      {
        return (c < '0' || c > '9') && (c < 'a' || c > 'z') && c != '_';
      });
  outputname.erase(it, outputname.end());

  // Add suffix
  outputname += ".out";
}

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath)
{
  std::map<std::string, ConfigFile> loadedConfigs;
  return GetConfigFile(filepath, loadedConfigs);
}

std::optional<ConfigFile> ConfigFile::GetConfigFile(const std::string& filepath, std::map<std::string, ConfigFile>& loadedConfigs)
{
  std::string realPath = FileUtils::GetRealPath(filepath);
  auto it = loadedConfigs.find(realPath);
  if(it != loadedConfigs.end())
  {
    return {};
  }

  std::ifstream f(filepath + CONFIG_FILENAME);
  // Check if the file exists
  if(f.good())
  {
    f.close();
    ConfigFile conf = ConfigFile::Load(realPath);
    loadedConfigs.emplace(realPath, conf);

    // Create dependency config files.
    for(size_t i = 0; i < conf.dependencies.size();++i)
    {
      std::optional<ConfigFile> dep = GetConfigFile(conf.configPath + conf.dependencies[i], loadedConfigs);
      if(dep)
      {
        conf.dependencyConfigs.push_back(*dep);
        conf.dependencies[i] = dep->configPath;
      }
      else
      {
        // Remove the dependency since it is already accounted for
        conf.dependencies.erase(conf.dependencies.begin() + i);
        --i;
      }
    }
    return conf;
  }
  return {};
}


ConfigFile ConfigFile::Load(const std::string& filepath)
{
  ConfigFile conf;
  conf.configPath = filepath;
  unsigned int loadFlag = 0;

  std::vector<std::string>* vec;
  std::string* s;
  bool* b;

  bool isDirectory = false;

  std::ifstream file(filepath+CONFIG_FILENAME);
  std::string line;

  if(file.is_open())
  {
    // config name, { pointer to memory, isDirectory}
    std::map<std::string, std::pair<std::string*, bool>> strings =
    {
      {"#srcdir", {&conf.srcdir, true}},
      {"#outputdir", {&conf.outputdir, true}},
      {"#outputname", {&conf.outputname, false}},
      {"#projectname", {&conf.projectname, false}},
      {"#hfile", {&conf.hFile, false}},
    };

    // config name, { pointer to memory, isDirectory}
    std::map<std::string, std::pair<std::vector<std::string>*, bool>> vectors =
    {
      {"#libs", {&conf.libs, false}},
      {"#libdirs", {&conf.libdirs, true}},
      {"#includedirs", {&conf.includedirs, true}},
      {"#compileflags", {&conf.flags, false}},
      {"#defines", {&conf.defines, false}},
      {"#dependencies", {&conf.dependencies, true}},
    };

    std::map<std::string, bool*> booleans =
    {
      {"#executable", &conf.executable},
      {"#shared", &conf.shared},
      {"#generatehfile", &conf.generateHFile},
    };

    while(std::getline(file,line))
    {
      if(line == "")
        continue;
      if(line[0]=='#')
      {
        // The format is a bit wacky, but it is this way since we do not want
        // to use map::find for all maps. This way we gain some optimization.
        auto&& itStr{strings.find(line)};
        if(itStr != strings.end())
        {
          s = itStr->second.first;
          isDirectory = itStr->second.second;
          loadFlag = FLAG_STRING;
        }
        else
        {
          auto&& itVec{vectors.find(line)};
          if(itVec != vectors.end())
          {
            vec = itVec->second.first;
            isDirectory = itVec->second.second;
            loadFlag = FLAG_VECTOR;
          }
          else
          {
            auto&& itBool{booleans.find(line)};
            if(itBool != booleans.end())
            {
              b = itBool->second;
              loadFlag = FLAG_BOOL;
            }
            else
            {
              LOG_ERROR("Invalid flag: ", line);
              loadFlag = FLAG_NONE;
            }
          }
        }
      }
      else
      {
        if(loadFlag == FLAG_STRING)
        {
          if(isDirectory && line[line.size()-1] != '/')
            line += '/';
          *s = line;
        }
        else if(loadFlag == FLAG_VECTOR)
        {
          if(isDirectory && line[line.size()-1] != '/')
          {;
            line += '/';
          }
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
    InputMultiple("Enter project dependencies:", conf.dependencies,true);
  }
  else
  {
    InputBoolean("Should it be compiled as a shared library? (y/n)", conf.shared);
    InputBoolean("Should it compile a project h-file? (y/n):", conf.generateHFile);
    if(conf.generateHFile)
    {
      InputString("Enter the project h-file name (relative to source directory): ", conf.hFile, false, false);
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
  file << "#dependencies" << std::endl;
  for(auto it = dependencies.begin();it!=dependencies.end();++it)
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
