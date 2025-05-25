#pragma once

#include "Common.h"
#include "FileUtils.h"

#include <assert.h>
#include <map>
#include <vector>
#include <string>

struct ConfigCache
{
  std::map<std::string, std::string> strings;
  std::map<std::string, std::vector<std::string>> vecStrings;
  std::map<std::string, bool> bools;
};

enum class ConfigSetting
{
  // vectors
  Library = 0, LibraryDir = 1, IncludeDir = 2, Define = 3, Dependency = 4, CFlag = 5, LFlag = 6, ExcludeSource = 7, ExcludeHeader = 8, ExecPreArgument = 9, ExecArgument = 10, SourceFile = 11,
  // Strings
  SourceDir = 32, OutputDir = 33, OutputName = 34, OutputType = 35, ProjectName = 36, HFileName = 37,
  // Bools
  GenerateHFile = 64,
  // Other
  Invalid = 1024
};

struct ConfigUtils
{
  static std::string GetSettingName(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::SourceDir:
        return "srcdir";
      case ConfigSetting::OutputDir:
        return "outputdir";
      case ConfigSetting::OutputName:
        return "outputname";
      case ConfigSetting::OutputType:
        return "outputtype";
      case ConfigSetting::ProjectName:
        return "projectname";
      case ConfigSetting::HFileName:
        return "hfilename";
      case ConfigSetting::LibraryDir:
        return "librarydir";
      case ConfigSetting::IncludeDir:
        return "includedir";
      case ConfigSetting::Dependency:
        return "dependency";
      case ConfigSetting::Library:
        return "library";
      case ConfigSetting::Define:
        return "define";
      case ConfigSetting::CFlag:
        return "cflag";
      case ConfigSetting::LFlag:
        return "lflag";
      case ConfigSetting::ExcludeSource:
        return "excludesource";
      case ConfigSetting::ExcludeHeader:
        return "excludeheader";
      case ConfigSetting::ExecPreArgument:
        return "preargument";
      case ConfigSetting::ExecArgument:
        return "argument";
      case ConfigSetting::GenerateHFile:
        return "generatehfile";
      case ConfigSetting::SourceFile:
        return "sourcefile";
      case ConfigSetting::Invalid:
        return "invalid";
    }
    return "";
  }

  static bool IsDirectory(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::SourceDir:
      case ConfigSetting::OutputDir:
      case ConfigSetting::LibraryDir:
      case ConfigSetting::IncludeDir:
      case ConfigSetting::Dependency:
        return true;
      case ConfigSetting::OutputName:
      case ConfigSetting::OutputType:
      case ConfigSetting::ProjectName:
      case ConfigSetting::HFileName:
      case ConfigSetting::Library:
      case ConfigSetting::Define:
      case ConfigSetting::CFlag:
      case ConfigSetting::LFlag:
      case ConfigSetting::ExcludeSource:
      case ConfigSetting::ExcludeHeader:
      case ConfigSetting::ExecPreArgument:
      case ConfigSetting::ExecArgument:
      case ConfigSetting::GenerateHFile:
      case ConfigSetting::SourceFile:
      case ConfigSetting::Invalid:
        return false;
    }
    return false;
  }

  static bool IsStringSetting(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::SourceDir:
      case ConfigSetting::OutputDir:
      case ConfigSetting::OutputName:
      case ConfigSetting::OutputType:
      case ConfigSetting::ProjectName:
      case ConfigSetting::HFileName:
        return true;
      case ConfigSetting::LibraryDir:
      case ConfigSetting::IncludeDir:
      case ConfigSetting::Dependency:
      case ConfigSetting::Library:
      case ConfigSetting::Define:
      case ConfigSetting::CFlag:
      case ConfigSetting::LFlag:
      case ConfigSetting::ExcludeHeader:
      case ConfigSetting::ExcludeSource:
      case ConfigSetting::ExecPreArgument:
      case ConfigSetting::ExecArgument:
      case ConfigSetting::GenerateHFile:
      case ConfigSetting::SourceFile:
      case ConfigSetting::Invalid:
        return false;
    }
    return false;
  }

  static bool IsVectorSetting(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::LibraryDir:
      case ConfigSetting::IncludeDir:
      case ConfigSetting::Dependency:
      case ConfigSetting::Library:
      case ConfigSetting::Define:
      case ConfigSetting::CFlag:
      case ConfigSetting::LFlag:
      case ConfigSetting::ExcludeHeader:
      case ConfigSetting::ExcludeSource:
      case ConfigSetting::ExecPreArgument:
      case ConfigSetting::ExecArgument:
      case ConfigSetting::SourceFile:
        return true;
      case ConfigSetting::SourceDir:
      case ConfigSetting::OutputDir:
      case ConfigSetting::OutputName:
      case ConfigSetting::OutputType:
      case ConfigSetting::ProjectName:
      case ConfigSetting::HFileName:
      case ConfigSetting::GenerateHFile:
      case ConfigSetting::Invalid:
        return false;
    }
    return false;
  }
  static bool IsBoolSetting(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::GenerateHFile:
        return true;
      case ConfigSetting::SourceDir:
      case ConfigSetting::OutputDir:
      case ConfigSetting::OutputName:
      case ConfigSetting::OutputType:
      case ConfigSetting::ProjectName:
      case ConfigSetting::HFileName:
      case ConfigSetting::LibraryDir:
      case ConfigSetting::IncludeDir:
      case ConfigSetting::Dependency:
      case ConfigSetting::Library:
      case ConfigSetting::Define:
      case ConfigSetting::CFlag:
      case ConfigSetting::LFlag:
      case ConfigSetting::ExcludeHeader:
      case ConfigSetting::ExcludeSource:
      case ConfigSetting::ExecPreArgument:
      case ConfigSetting::ExecArgument:
      case ConfigSetting::SourceFile:
      case ConfigSetting::Invalid:
        return false;
    }
    return false;
  }

  static std::string GetDefaultSettingString(ConfigSetting setting, const std::string& path)
  {
    switch(setting)
    {
      case ConfigSetting::SourceDir:
        return "src/";
      case ConfigSetting::OutputDir:
        return "bin/";
      case ConfigSetting::OutputName:
        return GetDefaultOutputName(path);
      case ConfigSetting::OutputType:
        return "executable";
      case ConfigSetting::ProjectName:
        return GetDefaultProjectName(path);
      case ConfigSetting::HFileName:
        return GetDefaultHFileName(path);
      case ConfigSetting::GenerateHFile:
        return GetDefaultSettingBool(setting) ? "true" : "false";
      default:
        LOG_ERROR("INVALID STRING ENUM: ", (int)setting);
        assert(false);
    }
    return "";
  }

  static bool GetDefaultSettingBool(ConfigSetting setting)
  {
    switch(setting)
    {
      case ConfigSetting::GenerateHFile:
        return false;
      default:
        LOG_ERROR("NOT BOOLEAN VALUE: ", (int)setting);
        assert(false);
    }
  }

  static std::string GetDefaultProjectName(const std::string& path)
  {
    return FileUtils::GetTopDirectory(path);
  }

  static std::string GetDefaultOutputName(const std::string& path)
  {
    std::string projectname = GetDefaultProjectName(path);
    std::string outputname;
    std::transform(
        projectname.begin(),
        projectname.end(),
        std::back_inserter(outputname),
        [](unsigned char c)
        {
          if(c == ' ')
            return '_';
          return (char)std::tolower(c);
        });
    auto it = std::remove_if(
        outputname.begin(),
        outputname.end(),
        [](unsigned char c)
        {
          return (c < '0' || c > '9') && (c < 'a' || c > 'z') && c != '_';
        });
    outputname.erase(it, outputname.end());
    outputname += ".out";
    return outputname;
  }

  static std::string GetDefaultHFileName(const std::string& path)
  {
    std::string hfile = GetDefaultProjectName(path);
    auto it = std::remove_if(
        hfile.begin(),
        hfile.end(),
        [](unsigned char c)
        {
          return (c < 'a' || c > 'z') && (c < 'A' || c > 'Z');
        });
    hfile.erase(it, hfile.end());
    hfile += ".h";
    return hfile;
  }
};
