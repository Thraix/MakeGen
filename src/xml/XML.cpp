#include "XML.h"

#include "XMLException.h"

#include <fstream>
#include <algorithm>

XMLObject XML::FromString(const std::string& string, const std::string& filename="")
{
  int startLine = 1;
  int startPos = 0;
  // Remove version tag.
  if(string.find("<?") != std::string::npos)
  {
    startPos = string.find("?>") + 3;
    startLine = std::count(string.begin(), string.begin()+startPos, '\n') + 1;
  }
  return XMLObject(string, startPos, startLine,filename);
}

XMLObject XML::FromFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if(!file)
    throw XMLException("Could not read file \""+filename+"\"");
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::string buffer;
  buffer.reserve(size);
  while (!file.eof())
  {
    buffer += file.get();
  }
  return FromString(buffer, filename);
}
