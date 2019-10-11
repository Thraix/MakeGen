#include "Utils.h"

#include "FileUtils.h"

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

void Utils::GetCppFiles(const ConfigFile& conf, std::set<std::string>& cppFiles)
{
  std::vector<std::string> files;
  std::string path = conf.configPath + conf.srcdir;
  FileUtils::GetAllFiles(path, files);

  for(auto it = files.begin(); it!=files.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    if(extensionPos != std::string::npos)
    {
      std::string extension = it->substr(extensionPos+1);
      std::string filename = it->substr(path.length());
      if(extension == "cpp" || extension == "c")
      {
        cppFiles.emplace(filename);
      }
    }
  }
}

void Utils::GetCppAndHFiles(const ConfigFile& conf, std::set<HFile>& hFiles, std::set<std::string>& cppFiles)
{
  std::vector<std::string> files;
  std::string path = conf.configPath + conf.srcdir;
  FileUtils::GetAllFiles(path,files);
  // include paramenter with the path of the file
  // For example src/graphics/Window.h -> graphics/Window.h if src is a src folder 
  for(auto it = files.begin(); it!=files.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    if(extensionPos != std::string::npos)
    {
      std::string extension = it->substr(extensionPos+1);
      std::string filename = it->substr(path.length());
      if(extension == "cpp" || extension == "c")
      {
        cppFiles.emplace(filename);
      }
      else if(extension == "hpp" || extension == "h")
      {
        hFiles.emplace(HFile{filename,path,false});
      }
    }
  }

  for(size_t i = 0; i < conf.dependencies.size(); ++i)
  {
    GetHFiles(conf.dependencies[i], conf.dependencyConfigs[i], hFiles);
  }
}

void Utils::GetHFiles(const std::string& dependencyDir, const ConfigFile& conf, std::set<HFile>& hFiles)
{
  // TODO: Fix so that cyclic dependencies doesn't crash the tool.
  // Cyclic dependencies probably shouldn't exist.
  // so just warn the user that it does and terminate.
  for(size_t i = 0; i < conf.dependencies.size(); ++i)
  {
    GetHFiles(conf.dependencies[i], conf.dependencyConfigs[i], hFiles);
  }

  std::vector<std::string> files;
  std::string depSrcDir = dependencyDir + conf.srcdir;
  FileUtils::GetAllFiles(depSrcDir, files);
  for(auto it = files.begin(); it!=files.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    if(extensionPos != std::string::npos)
    {
      std::string extension = it->substr(extensionPos+1);
      if(extension == "hpp" || extension == "h")
      {
        std::string filename = it->substr(depSrcDir.length());
        hFiles.emplace(HFile{filename, depSrcDir, conf.generateHFile && filename == conf.hFile});
      }
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
