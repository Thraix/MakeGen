#pragma once

#include <iostream>
#include <set>
#include <map>
#include <string>
#include <fstream>
#include <exception>
#include "ConfigFile.h"
#include "FileUtils.h"

struct CompareIncludeDeps;

class IncludeDeps
{
  public:
    std::map<std::string, IncludeDeps*> dependencies;
    std::string filepath;
    static std::set<std::string> printSet;
    static int printCounter;

    IncludeDeps(const std::string& filename, const std::string& dir, const std::map<std::string, std::string>& files, std::map<std::string, IncludeDeps*>& allDeps);

    std::string GetIncludeFile(const std::string& line, size_t pos, const std::string& filename);

    std::ostream& Output(std::ostream& stream, const ConfigFile& conf)
    {
      if(printSet.find(filepath) != printSet.end())
        return stream;
      printCounter++; 
      printSet.emplace(filepath);
      stream << FileUtils::GetRelativePath(conf.configPath, filepath);
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

    IncludeDeps(const std::string& filename, const std::string& dir)
      : filepath(dir+filename){}

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
