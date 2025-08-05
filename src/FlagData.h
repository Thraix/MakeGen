#pragma once

#include <string>

struct FlagData
{
  unsigned int flags{0};
  std::string target{""}; // Only set if flags contain FLAG_TARGET
};
