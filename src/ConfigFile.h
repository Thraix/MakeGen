#pragma once

#include "ConfigUtils.h"
#include "xml/XMLObject.h"

#include <map>
#include <optional>
#include <string>
#include <vector>

static const std::string CONFIG_FILENAME = "makegen.xml";

class ConfigFile
{
  private:
    ConfigCache cache;

    XMLObject config;
    // Current configuration
    std::string target;

    std::string configPath;
    std::vector<ConfigFile> dependencyConfigs;

    bool hasInitError = false;

  public:
    // Generates a new default config file
    ConfigFile(const std::string& path, int);
    ConfigFile(const std::string& path);
    ConfigFile(XMLObject& config, const std::string& path);

    void Save() const;

    std::string& GetSettingString(ConfigSetting setting);
    bool GetSettingBool(ConfigSetting setting);
    std::vector<std::string>& GetSettingVectorString(ConfigSetting setting);
    std::vector<std::string> GetSetting(ConfigSetting setting);

    bool SetSettingString(ConfigSetting setting, const std::string& value);
    bool AddSettingVectorString(ConfigSetting setting, const std::string& value);
    bool RemoveSettingVectorString(ConfigSetting setting, const std::string& value);

    XMLObject& GetConfiguration();
    const std::string& GetConfigPath() const;
    ConfigFile& GetDependencyConfig(size_t i);
  private:
    void Init();

  public:
    static ConfigFile Gen();
    static std::optional<ConfigFile> GetConfigFile(const std::string& filepath = "./");
  private:
    static std::optional<ConfigFile> GetConfigFile(const std::string& filepath, std::map<std::string, ConfigFile>& loadedConfigs);
    static std::optional<ConfigFile> Load(const std::string& filename);
    static void InputBoolean(const std::string& inputText, bool& b);
    static void InputMultiple(const std::string& inputText, std::vector<std::string>& vec, bool needEnding);
    static void InputString(const std::string& inputText, std::string& vec, bool needEnding, bool allowEmpty);
};
