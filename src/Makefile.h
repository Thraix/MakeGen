#pragma once

#include "ConfigFile.h"
#include <set>
#include <map>

class Makefile
{
  public:
    static void Save(const ConfigFile& conf);
  private:
    static void GetCppAndHFiles(const ConfigFile& conf, std::map<std::string, std::string>& hFiles, std::set<std::string>& cppFiles);
};
