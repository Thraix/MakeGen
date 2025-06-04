#pragma once

#include "ConfigFile.h"
#include "FileUtils.h"

#include <iostream>
#include <filesystem>
#include <map>
#include <set>
#include <string>

struct CompareIncludeDeps;

class IncludeDeps
{
  public:
    std::map<std::string, IncludeDeps*> dependencies;
    std::filesystem::path filepath;
    bool projectHFile;
    static std::set<std::string> printSet;
    static int printCounter;

    IncludeDeps(const std::string& filename, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps);

    IncludeDeps(const std::string& filename, bool projectHFile, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps);

    std::string GetIncludeFile(const std::string& line);

    std::ostream& Output(std::ostream& stream, const ConfigFile& conf)
    {
      std::string filePathInMakeFile = filepath;
      if(!filepath.is_absolute())
        filePathInMakeFile  = std::filesystem::relative(conf.GetConfigPath() + "/" + filepath.string(), "./").string();

      if(printSet.find(filePathInMakeFile) != printSet.end())
        return stream;
      printSet.emplace(filePathInMakeFile);
      printCounter++;

      if(!projectHFile)
      {
        stream << " " << filePathInMakeFile;
      }
      for(auto it = dependencies.begin(); it != dependencies.end(); ++it)
      {
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
