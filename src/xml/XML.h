#pragma once

#include "XMLObject.h"

struct XML
{
  static XMLObject FromString(const std::string& string, const std::string& filename);
  static XMLObject FromFile(const std::string& fileName);
};
