#pragma once

#include "ConfigFile.h"

class Makefile
{
  public:
    static void Save(ConfigFile& conf, unsigned int flags);
    static void GetIncludePaths(std::ostream& file, ConfigFile& conf, const std::string& confPath = "");

    static void GetLibraryPaths(std::ostream& file, ConfigFile& conf, const std::string& confPath = "");

    static void GetLibraries(std::ostream& file, ConfigFile& conf);

    static void GetDefines(std::ostream& file, ConfigFile& conf);
};
