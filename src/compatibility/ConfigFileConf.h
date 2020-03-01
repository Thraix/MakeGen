#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

class ConfigFile;

class ConfigFileConf
{
  public:
    std::string configPath;
    std::vector<std::string> libs;
    std::vector<std::string> libdirs;
    std::vector<std::string> includedirs;
    std::vector<std::string> defines;
    std::vector<std::string> flags;
    std::vector<std::string> dependencies;

    std::string outputdir;
    std::string srcdir;
    std::string outputname;
    std::string projectname;
    std::string hFile;
    bool executable;
    bool shared;
    bool generateHFile;
  public:
    ConfigFileConf();

    static void CreateXMLFile(const std::string& filename);
  private:
};
