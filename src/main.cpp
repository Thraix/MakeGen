#include "Common.h"
#include "ConfigCLI.h"
#include "ConfigFile.h"
#include "FileUtils.h"
#include "FlagData.h"
#include "HFileGen.h"
#include "Makefile.h"
#include "Timer.h"

#include <cmath>
#include <thread>
#include <filesystem>

#define RETURN_IF(x, b) \
  if(x)\
  return b;

void PrintHelp()
{
  LOG_INFO("MakeGen ", MAKEGEN_VERSION);
  LOG_INFO(1+(char*)R"(

MakeGen is a utility tool to generate and run Makefiles in a simple manner.
By default it always compiles code with parallell jobs.

Usage: makegen [options]

  Options:
    -h,     --help             Displays this information
    -v,     --version          Displays the version of this program
    -m,-a,  make, all          Generates a Makefile and runs
                                make all
    -i,     install            Generates a Makefile and runs
                                make all && make install
    -c,     clean              Generates a Makefile and runs
                                make clean
    -r,     rebuild            Generates a Makefile and runs
                                make clean && make all
    -e,     run, execute       Generates a Makefile and runs
                                make all && make run
    -s,     single             Runs additional makegen options as single thread
                                (no --jobs=X flag)
            --simple           Creates a simple Makefile without include dependencies
                                (no --jobs=X flag)
            --target=<target>  Run the makegen.xml file with the specified target

  If no option is given it will run \"make all\"

  If multiple make options are given it will run in the following order:
    clean all install run, rebuild will be translated to \"clean make\")");
}

void GenMakefile(ConfigFile& conf, unsigned int flags)
{
  if(conf.GetSettingBool(ConfigSetting::GenerateHFile))
    HFileGen::Create(conf);
  Makefile::Save(conf, flags);
}

FlagData ReadFlags(int argc, char** argv)
{
  if(argc >= 2 && std::string(argv[1]) == "conf")
    return FlagData{FLAG_CONFIG};
  FlagData flagData{};
  bool make = true;
  for(int i = 1;i<argc;i++)
  {
    if(strlen(argv[i]) > 1)
    {
      std::string flag(argv[i]);
      if(flag == "-h" || flag == "--help")
      {
        flagData.flags |= FLAG_HELP;
      }
      else if(flag == "-v" || flag == "--version")
      {
        flagData.flags |= FLAG_VERSION;
      }
      else if(flag == "make" || flag == "-m" || flag == "all" || flag == "-a")
      {
        flagData.flags |= FLAG_MAKE;
      }
      else if(flag == "clean" || flag == "-c")
      {
        make = false;
        flagData.flags |= FLAG_CLEAN;
      }
      else if(flag == "run" || flag == "-e" || flag == "execute")
      {
        flagData.flags |= FLAG_RUN;
      }
      else if(flag == "install" || flag == "-i")
      {
        flagData.flags |= FLAG_INSTALL;
      }
      else if(flag == "rebuild" || flag == "-r")
      {
        flagData.flags |= FLAG_CLEAN;
        flagData.flags |= FLAG_MAKE;
      }
      else if(flag == "single" || flag == "-s")
      {
        flagData.flags |= FLAG_SINGLE_THREAD;
      }
      else if(flag == "--simple")
      {
        flagData.flags |= FLAG_SIMPLE;
      }
      else if(Utils::StartsWith(flag, "--target="))
      {
        std::string prefix("--target=");
        if (flag.size() < prefix.size() + 1)
        {
          LOG_ERROR("No target specified in --target=<target>");
          return FlagData{FLAG_HELP};
        }
        flagData.flags |= FLAG_TARGET;
        flagData.target = flag.substr(std::string("--target=").size());
      }
      else if(flag != "")
      {
        LOG_ERROR("Unknown argument ", flag);
        return FlagData{FLAG_HELP};
      }
    }
  }
  if(make)
    flagData.flags |= FLAG_MAKE;
  return flagData;
}

bool RunMake(const std::string& filepath, unsigned int flags, ConfigFile& conf)
{
  std::string make = "make --no-print-directory -C " + filepath;
  if(!(flags & FLAG_SINGLE_THREAD))
    make += " -j" + std::to_string(std::thread::hardware_concurrency()) + " ";
  if(flags & FLAG_CLEAN)
  {
    RETURN_IF(system(std::string(make + " clean").c_str()) != 0, false);
  }
  if(flags & FLAG_MAKE)
  {
    RETURN_IF(system(std::string(make + " all").c_str()) != 0, false);
  }
  if(flags & FLAG_INSTALL)
  {
    RETURN_IF(system(std::string(make + " install").c_str()) != 0, false);
  }
  if(flags & FLAG_RUN && conf.GetSettingString(ConfigSetting::OutputType) == "executable")
  {
    RETURN_IF(system(std::string(make + " run").c_str()) != 0, false);
  }
  return true;
}

bool MakeGen(const std::string& filepath, const FlagData& flagData, ConfigFile& conf)
{
  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(size_t i = 0;i<dependencies.size();++i)
  {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::current_path(dependencies[i]);

    auto conf = ConfigFile::GetConfigFile("./", flagData);
    if(conf)
    {
      bool success = MakeGen("./", flagData, conf.value());
      if(!success)
      {
        std::filesystem::current_path(currentPath);
        return success;
      }
    }
    std::filesystem::current_path(currentPath);
  }
  LOG_INFO("-----------------------------------");
  LOG_INFO("Building ", conf.GetSettingString(ConfigSetting::ProjectName));
  LOG_INFO("Generating Makefile...");
  Timer timer;
  GenMakefile(conf, flagData.flags);
  LOG_INFO("Took ", round(timer.Elapsed()*1000.0)/1000.0,"s");
  LOG_INFO("Running Makefile...");

  std::string outputPath = conf.GetConfigPath() + conf.GetSettingString(ConfigSetting::OutputDir);
  if(!FileUtils::HasPath(outputPath))
  {
    FileUtils::CreateDirectory(outputPath);
    std::string intermediatePath = outputPath + "intermediates";
    if(!FileUtils::HasPath(intermediatePath ))
      FileUtils::CreateDirectory(intermediatePath );
  }
  return RunMake(filepath, flagData.flags, conf);
}

int main(int argc, char** argv)
{
  FlagData flagData = ReadFlags(argc,argv);
  if(flagData.flags & FLAG_HELP)
  {
    PrintHelp();
    return 0;
  }
  if(flagData.flags & FLAG_VERSION)
  {
    LOG_INFO("MakeGen ", MAKEGEN_VERSION);
    return 0;
  }
  if(flagData.flags & FLAG_CONFIG)
  {
    return ConfigCLI::Main(argc-1, &argv[1]);
  }
  std::map<std::string, ConfigFile> files{};
  auto conf = ConfigFile::GetConfigFile("./", flagData);
  if(conf)
  {
    bool success = MakeGen("./", flagData, *conf);
    return success ? 0 : 1;
  }
  else
  {
    LOG_ERROR("Couldn\'t load config file");
  }
}
