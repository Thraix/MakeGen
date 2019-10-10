#pragma once

#include "ConfigFile.h"

struct ConfigCLI 
{
  public:
    static int Main(int argc, char** argv);
  private:
    static void DisplayCLIHelp();
    static void DisplayGenHelp();
    static void DisplayAddHelp();
    static void DisplayRemoveHelp();
    static void DisplaySetHelp();
    static void DisplayGetHelp();

    static std::map<std::string, std::vector<std::string>*> GetSettingVectorMap(ConfigFile& config); 
    static std::map<std::string, std::string*> GetSettingStringMap(ConfigFile& config); 
    static std::map<std::string, bool*> GetSettingBoolMap(ConfigFile& config); 

    static int Gen(int argc, char** argv);
    static int Add(int argc, char** argv, ConfigFile& config);
    static int Remove(int argc, char** argv, ConfigFile& config);
    static int Set(int argc, char** argv, ConfigFile& config);
    static int Get(int argc, char** argv, ConfigFile& config);

};
