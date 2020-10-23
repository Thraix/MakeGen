#pragma once

#include "ConfigFile.h"
#include "FileUtils.h"

#include <iostream>
#include <map>
#include <set>
#include <string>

struct CompareIncludeDeps;

class IncludeDeps
{
  public:
    std::map<std::string, IncludeDeps*> dependencies;
    std::string filepath;
    bool projectHFile;
    static std::set<std::string> printSet;
    static int printCounter;

    IncludeDeps(const std::string& filename, const std::string& dir, const std::set<IncludeFile>& files, std::map<std::string, IncludeDeps*>& allDeps);

    IncludeDeps(const std::string& filename, const std::string& dir, bool projectHFile, const std::set<IncludeFile>& files, std::map<std::string, IncludeDeps*>& allDeps);

    std::string GetIncludeFile(const std::string& line, size_t pos, const std::string& filename);

    std::ostream& Output(std::ostream& stream, const ConfigFile& conf)
    {
      if(printSet.find(filepath) != printSet.end())
        return stream;
      printCounter++;
      printSet.emplace(filepath);
      if(!projectHFile)
        stream << FileUtils::GetRelativePath(conf.GetConfigPath(), filepath);
      for(auto it = dependencies.begin();it!=dependencies.end();++it)
      {
        stream << " ";
        (it->second)->Output(stream, conf);
      }
      printCounter--;
      if(printCounter == 0)
        printSet.clear();
      return stream;
    }
};

struct CompareIncludeDeps
{
  using is_transparent = void;
  bool operator()(const IncludeDeps* d1, const IncludeDeps* d2) const
  {
    return d1->filepath < d2->filepath;
  }
  bool operator()(const IncludeDeps* d, const std::string& filepath) const
  {
    return d->filepath < filepath;
  }
  bool operator()(const std::string& filepath, const IncludeDeps* d) const
  {
    return filepath < d->filepath;
  }
};
