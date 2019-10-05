#pragma once

#include "ConfigFile.h"

#include <set>
#include <map>

class Makefile
{
  public:
    static void Save(const ConfigFile& conf, unsigned int flags);
};
