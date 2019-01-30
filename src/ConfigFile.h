#pragma once

#include <vector>
#include <string>

class ConfigFile
{
  public:
    std::vector<std::string> libs;
    std::vector<std::string> libdirs;
    std::vector<std::string> includedirs;
    std::vector<std::string> defines;
    std::vector<std::string> flags;
    std::string outputdir;
    std::string srcdir;
    std::string outputname;
    std::string projectname;
    std::string hFile;
    bool executable;
    bool shared;
    bool generateHFile;
  public:
    ConfigFile();
    void Save() const;
    static ConfigFile Gen();
    static ConfigFile Load(const std::string& filename);
  private:
    static void InputBoolean(const std::string& inputText, bool& b);
    static void InputMultiple(const std::string& inputText, std::vector<std::string>& vec, bool needEnding);
    static void InputString(const std::string& inputText, std::string& vec, bool needEnding, bool allowEmpty);
};
