#pragma once

#include <filesystem>
#include <iostream>
#include <set>
#include <string>


struct HFile
{
  std::string filename;
  bool isProjectHFile;

  std::filesystem::path filepath;

  HFile(const std::string& filename, const std::string& directory, bool isProjectHFile)
    : filename{filename}, isProjectHFile{isProjectHFile}, filepath{directory+filename}
  {}

  friend bool operator<(const HFile& h1, const HFile& h2)
  {
    return h1.filename < h2.filename;
  }

  friend std::ostream& operator<<(std::ostream& ostream, const HFile& hFile)
  {
    return ostream << "filename: " << hFile.filename << "\tfilepath: " << hFile.filepath.string();
  }
};

class ConfigFile;

struct Utils
{
  static bool IsSourceFile(const std::string& filepath);
  static bool IsHeaderFile(const std::string& filepath);

  static std::string CommonPrefix(const std::string& s1, const std::string& s2);
  static bool StartsWith(const std::string& str, const std::string& prefix);
  static void GetCppFiles(ConfigFile& conf, std::set<std::string>& cppFiles);
  static void GetCppAndHFiles(ConfigFile& conf, std::set<HFile>& hFiles, std::set<std::string>& cppFiles);
  static void GetHFiles(const std::string& dependencyDir, ConfigFile& conf, std::set<HFile>& hFiles);

  // Used for parsing xml
  static bool IsWhiteSpace(char c);
  static bool IsLetter(char c);
  static bool IsWord(const std::string& string);
  static std::string GetWord(const std::string& string, int startPos = 0);
};
