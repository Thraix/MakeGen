#pragma once

#include <vector>
#include <string>

static const std::string CONFIG_FILENAME = "makegen.conf";

class ConfigFile
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
    std::vector<ConfigFile> dependencyConfigs;
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
