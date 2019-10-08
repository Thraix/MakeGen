#pragma once

#include "ConfigFile.h"

class Makefile
{
  public:
    static void Save(const ConfigFile& conf, unsigned int flags);
};
