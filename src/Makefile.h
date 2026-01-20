#pragma once

#include "ConfigFile.h"

class Makefile
{
public:
  static void Save(ConfigFile& conf, unsigned int flags);

private:
  static std::set<std::string> GetIntermediateDirectories(const std::set<std::string>& cppFiles);
};
