#pragma once

#include <vector>
#include <string>

class ConfigFile
{
  public:
    std::vector<std::string> libs;
    std::vector<std::string> libdirs;
    std::vector<std::string> includedirs;
    std::vector<std::string> srcdirs;
    std::vector<std::string> defines;
    std::string outputdir;
    std::string outputname;
    bool executable;
  public:
    ConfigFile();
    void Save() const;
    static ConfigFile Gen();
    static ConfigFile Load();
};
