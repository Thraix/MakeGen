#pragma once

#include <set>
#include <string>


struct IncludeFile
{
  std::string filename;
  std::string directory;
  bool isProjectHFile;

  std::string filepath;

  IncludeFile(const std::string& filename, const std::string& directory, bool isProjectHFile)
    : filename{filename}, directory{directory}, isProjectHFile{isProjectHFile}, filepath{directory+filename}
  {}

  friend bool operator<(const IncludeFile& h1, const IncludeFile& h2)
  {
    return h1.filename < h2.filename;
  }
};

class ConfigFile;

struct Utils
{
  static bool IsSourceFile(const std::string& filepath);
  static bool IsIncludeFile(const std::string& filepath);
  static std::string CommonPrefix(const std::string& s1, const std::string& s2);
  static void GetSourceFiles(ConfigFile& conf, std::set<std::string>& cppFiles);
  static void GetSourceAndIncludeFiles(ConfigFile& conf, std::set<IncludeFile>& IncludeFiles, std::set<std::string>& cppFiles);
  static void GetIncludeFiles(const std::string& dependencyDir, ConfigFile& conf, std::set<IncludeFile>& IncludeFiles);

  // Used for parsing xml
  static bool IsWhiteSpace(char c);
  static bool IsLetter(char c);
  static bool IsWord(const std::string& string);
  static std::string GetWord(const std::string& string, int startPos = 0);
};
