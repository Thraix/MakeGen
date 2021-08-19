#include "Utils.h"

#include "ConfigFile.h"
#include "FileUtils.h"

bool Utils::IsSourceFile(const std::string& filepath)
{
  std::string_view extension(filepath);
  size_t pSlash = filepath.find_last_of('/');
  size_t pDot = filepath.find_last_of('.');
  if(pDot == std::string::npos || (pSlash != std::string::npos && pSlash > pDot))
  {
    LOG_ERROR("No file extension for file: ", filepath);
    return false;
  }
  extension.remove_prefix(pDot + 1);
  return extension == "cpp" || extension == "c" || extension == "cxx" || extension == "cc";
}

bool Utils::IsIncludeFile(const std::string& filepath)
{
  return !IsSourceFile(filepath);
}

bool Utils::IsHeaderFile(const std::string& filepath)
{
  std::string_view extension(filepath);
  size_t pSlash = filepath.find_last_of('/');
  size_t pDot = filepath.find_last_of('.');
  if(pDot == std::string::npos || (pSlash != std::string::npos && pSlash > pDot))
  {
    LOG_ERROR("No file extension for file: ", filepath);
    return false;
  }
  extension.remove_prefix(pDot + 1);
  return extension == "hpp" || extension == "h" || extension == "hxx" || extension == "hh";
}

std::string Utils::CommonPrefix(const std::string& s1, const std::string& s2)
{
  size_t n = 0;
  for(size_t i = 0; i<s1.size() && i<s2.size();++i)
  {
    if(s1[i] != s2[i])
    {
      n = i;
      break;
    }
  }
  return s1.substr(0, n);
}

void Utils::GetSourceFiles(ConfigFile& conf, std::set<std::string>& cppFiles)
{
  std::vector<std::string> files;
  std::string path = conf.GetConfigPath() + conf.GetSettingString(ConfigSetting::SourceDir);
  FileUtils::GetAllFiles(path, files);
  const std::vector<std::string>& excludeSources = conf.GetSettingVectorString(ConfigSetting::ExcludeSource);

  for(auto it = files.begin(); it!=files.end();++it)
  {
    std::string filename = it->substr(path.length());
    if(IsSourceFile(filename))
    {
      std::string sourceFile =conf.GetSettingString(ConfigSetting::SourceDir) + filename;
      auto it = std::find(excludeSources.begin(), excludeSources.end(), sourceFile);
      if(it == excludeSources.end())
      {
        cppFiles.emplace(filename);
      }
    }
  }
}


void Utils::GetSourceAndIncludeFiles(ConfigFile& conf, std::set<IncludeFile>& hFiles, std::set<std::string>& cppFiles)
{
  std::vector<std::string> files;
  std::string path = conf.GetConfigPath() + conf.GetSettingString(ConfigSetting::SourceDir);
  FileUtils::GetAllFiles(path,files);
  const std::vector<std::string>& excludeSources = conf.GetSettingVectorString(ConfigSetting::ExcludeSource);
  // include paramenter with the path of the file
  // For example src/graphics/Window.h -> graphics/Window.h if src is a src folder
  for(auto it = files.begin(); it!=files.end();++it)
  {
    std::string filename = it->substr(path.length());
    if(IsSourceFile(filename))
    {
      std::string sourceFile = conf.GetSettingString(ConfigSetting::SourceDir) + filename;
      auto it = std::find(excludeSources.begin(), excludeSources.end(), sourceFile);
      if(it == excludeSources.end())
      {
        cppFiles.emplace(filename);
      }
    }
    else
    {
      hFiles.emplace(IncludeFile{filename,path,false});
    }
  }

  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(size_t i = 0; i < dependencies.size(); ++i)
  {
    GetIncludeFiles(dependencies[i], conf.GetDependencyConfig(i), hFiles);
  }
}

void Utils::GetIncludeFiles(const std::string& dependencyDir, ConfigFile& conf, std::set<IncludeFile>& hFiles)
{
  // TODO: Fix so that cyclic dependencies doesn't crash the tool.
  // Cyclic dependencies probably shouldn't exist.
  // so just warn the user that it does and terminate.
  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(size_t i = 0; i < dependencies.size(); ++i)
  {
    GetIncludeFiles(dependencies[i], conf.GetDependencyConfig(i), hFiles);
  }

  std::vector<std::string> files;
  std::string depSrcDir = dependencyDir + conf.GetSettingString(ConfigSetting::SourceDir);
  FileUtils::GetAllFiles(depSrcDir, files);
  for(auto it = files.begin(); it!=files.end();++it)
  {
    if(IsIncludeFile(*it))
    {
      std::string filename = it->substr(depSrcDir.length());
      hFiles.emplace(IncludeFile{filename, depSrcDir, conf.GetSettingBool(ConfigSetting::GenerateHFile) && filename == conf.GetSettingString(ConfigSetting::HFileName)});
    }
  }
}
bool Utils::IsWhiteSpace(char c)
{
  return c == '\n' || c == '\t' || c == '\r' || c == ' ' || c == '\t';
}

bool Utils::IsLetter(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Utils::IsWord(const std::string& string)
{
  for(auto it{string.begin()}; it != string.end();++it)
  {
    if(!IsLetter(*it))
      return false;
  }
  return true;
}

std::string Utils::GetWord(const std::string& string, int startPos)
{
  if (startPos >= string.length())
    throw std::runtime_error("start position out of bounds.");

  int endPos = startPos;
  while (endPos < string.length() && IsLetter(string[endPos])) endPos++;
  return string.substr(startPos, endPos - startPos);
}
