#pragma once

#include "ConfigFile.h"

#include <map>
#include <set>
#include <string>


struct HFile
{
  std::string filename;
  std::string directory;
  bool isProjectHFile;

  std::string filepath;

  HFile(const std::string& filename, const std::string& directory, bool isProjectHFile)
    : filename{filename}, directory{directory}, isProjectHFile{isProjectHFile}, filepath{directory+filename}
  {}

  friend bool operator<(const HFile& h1, const HFile& h2)
  {
    return h1.filename < h2.filename;
  }
};

struct Utils
{
  static std::string CommonPrefix(const std::string& s1, const std::string& s2);
  static void GetCppFiles(const ConfigFile& conf, std::set<std::string>& cppFiles);
  static void GetCppAndHFiles(const ConfigFile& conf, std::set<HFile>& hFiles, std::set<std::string>& cppFiles);
  static void GetHFiles(const std::string& dependencyDir, const ConfigFile& conf, std::set<HFile>& hFiles);
};
