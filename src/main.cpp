#include "CLI.h"
#include "Common.h"
#include "ConfigFile.h"
#include "FileUtils.h"
#include "HFileGen.h"
#include "Makefile.h"
#include "Timer.h"

#include <cmath>
#include <thread>

#define RETURN_IF(x, b) \
  if(x)\
  return b;

void PrintHelp()
{
  LOG_INFO("MakeGen ", MAKEGEN_VERSION);
  LOG_INFO("");
  LOG_INFO("MakeGen is a utility tool to generate and run Makefiles in a simple manner.\nBy default it always compiles code with parallell jobs.");
  LOG_INFO("");
  LOG_INFO("Usage: makegen [options]");
  LOG_INFO("");
  LOG_INFO("  Options:");
  LOG_INFO("    -h,     --help        Displays this information");
  LOG_INFO("            conf          Run config command with given parameters. To see more");
  LOG_INFO("                           run makegen conf --help");
  LOG_INFO("    -v,     --version     Displays the version of this program");
  LOG_INFO("    -m,-a,  make, all     Generates a Makefile and runs");
  LOG_INFO("                           make all");
  LOG_INFO("    -i,     install       Generates a Makefile and runs");
  LOG_INFO("                           make all && make install");
  LOG_INFO("    -c,     clean         Generates a Makefile and runs");
  LOG_INFO("                           make clean");
  LOG_INFO("    -r,     rebuild       Generates a Makefile and runs");
  LOG_INFO("                           make clean && make all");
  LOG_INFO("    -e,     run, execute  Generates a Makefile and runs");
  LOG_INFO("                           make all && make run");
  LOG_INFO("    -s,     single        Runs additional makegen options as single thread");
  LOG_INFO("                           (no --jobs=X flag)");
  LOG_INFO("            --simple      Creates a simple Makefile without include dependencies");
  LOG_INFO("                           (no --jobs=X flag)");
  LOG_INFO("");
  LOG_INFO("  If no option is given it will run \"make all\"");
  LOG_INFO("");
  LOG_INFO("  If multiple make options are given it will run in the following order:");
  LOG_INFO("    clean all install run, rebuild will be translated to \"clean make\"");
}

void GenMakefile(const ConfigFile& conf, unsigned int flags)
{
  if(conf.generateHFile)
    HFileGen::Create(conf);
  Makefile::Save(conf, flags);
}

unsigned int ReadFlags(int argc, char** argv)
{
  if(argc >= 2 && std::string(argv[1]) == "conf")
    return FLAG_CLI;
  unsigned int flags = 0;
  bool make = true;
  for(int i = 1;i<argc;i++)
  {
    if(strlen(argv[i]) > 1)
    {
      std::string flag(argv[i]);
      if(flag == "-h" || flag == "--help")
      {
        flags |= FLAG_HELP;
      }
      else if(flag == "--conf")
      {
        flags |= FLAG_GEN;
      }
      else if(flag == "-v" || flag == "--version")
      {
        flags |= FLAG_VERSION;
      }
      else if(flag == "make" || flag == "-m" || flag == "all" || flag == "-a")
      {
        flags |= FLAG_MAKE;
      }
      else if(flag == "clean" || flag == "-c")
      {
        make = false;
        flags |= FLAG_CLEAN;
      }
      else if(flag == "run" || flag == "-e" || flag == "execute")
      {
        flags |= FLAG_RUN;
      }
      else if(flag == "install" || flag == "-i")
      {
        flags |= FLAG_INSTALL;
      }
      else if(flag == "rebuild" || flag == "-r")
      {
        flags |= FLAG_CLEAN;
        flags |= FLAG_MAKE;
      }
      else if(flag == "single" || flag == "-s")
      {
        flags |= FLAG_SINGLE_THREAD;
      }
      else if(flag == "--simple")
      {
        flags |= FLAG_SIMPLE;
      }
      else if(flag != "")
      {
        LOG_ERROR("Unknown argument ", flag);
        return FLAG_HELP;
      }
    }
  }
  if(make)
    flags |= FLAG_MAKE;
  return flags;
}

bool RunMake(const std::string& filepath, unsigned int flags, const ConfigFile& conf)
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
  if(flags & FLAG_RUN && conf.executable)
  {
    RETURN_IF(system(std::string(make + " run").c_str()) != 0, false);
  }
  return true;
}

bool MakeGen(const std::string& filepath, unsigned int flags, const ConfigFile& conf)
{
  for(size_t i = 0;i<conf.dependencies.size();++i)
  {
    bool success = MakeGen(conf.dependencies[i], flags, conf.dependencyConfigs[i]);
    if(!success)
      return success;
  }
  LOG_INFO("-----------------------------------");
  LOG_INFO("Building ", conf.projectname);
  LOG_INFO("Generating Makefile...");
  Timer timer;
  GenMakefile(conf, flags);
  LOG_INFO("Took ", round(timer.Elapsed()*1000.0)/1000.0,"s");
  LOG_INFO("Running Makefile...");

  if(!FileUtils::HasPath(conf.configPath + conf.outputdir))
  {
    FileUtils::CreateDirectory(conf.configPath + conf.outputdir);
    FileUtils::CreateDirectory(conf.configPath + conf.outputdir + "intermediates");
  }
  return RunMake(filepath, flags, conf);
}

int main(int argc, char** argv)
{
  unsigned int flags = ReadFlags(argc,argv);
  if(flags & FLAG_HELP)
  {
    PrintHelp();
    return 0;
  }
  if(flags & FLAG_VERSION)
  {
    LOG_INFO("MakeGen ",MAKEGEN_VERSION);
    return 0;
  }
  if(flags & FLAG_GEN)
  {
    ConfigFile::Gen().Save();
    return 0;
  }
  if(flags & FLAG_CLI)
  {
    return CLI::Main(argc-1, &argv[1]);
  }
  std::map<std::string, ConfigFile> files{};
  auto conf = ConfigFile::GetConfigFile();
  if(conf)
  {
    bool success = MakeGen("./", flags, *conf);
    return success ? 0 : 1;
  }
  else
  {
    LOG_ERROR("No ", CONFIG_FILENAME, " or Makefile found.");
    PrintHelp();
  }
}
