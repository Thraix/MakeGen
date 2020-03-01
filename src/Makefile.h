#pragma once

#include "ConfigFile.h"

class Makefile
{
  public:
    static void Save(ConfigFile& conf, unsigned int flags);
};
