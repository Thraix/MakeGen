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
    std::vector<std::string> flags;
    std::string outputdir;
    std::string outputname;
    std::string projectname;
    bool executable;
    bool shared;
  public:
    ConfigFile();
    void Save() const;
    static ConfigFile Gen();
    static ConfigFile Load();
  private:
    static void InputMultiple(const std::string& inputText, std::vector<std::string>& vec, bool needEnding);
};
