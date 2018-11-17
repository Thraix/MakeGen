#pragma once

#include "ConfigFile.h"
#include <map>

class Makefile
{
  public:
    static void Save(const ConfigFile& conf);
  private:
    static void PreSave(const ConfigFile& conf, std::map<std::string, std::string>& hFiles, std::map<std::string, std::string>& cppFiles);
    static void GetAllFiles(const std::string& folder, std::vector<std::string>& files);
};
