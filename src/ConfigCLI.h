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

    static ConfigSetting CLIStringToSetting(const std::string& s);

    static int Gen(int argc, char** argv);
    static int Add(int argc, char** argv, ConfigFile& config);
    static int Remove(int argc, char** argv, ConfigFile& config);
    static int Set(int argc, char** argv, ConfigFile& config);
    static int Get(int argc, char** argv, ConfigFile& config);

};
