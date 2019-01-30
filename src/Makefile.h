#pragma once

#include "ConfigFile.h"
#include <set>

class Makefile
{
  public:
    static void Save(const ConfigFile& conf);
  private:
    static void PreSave(const ConfigFile& conf, std::set<std::string>& hFiles, std::set<std::string>& cppFiles);
};
