#include "Makefile.h"

#include "IncludeDeps.h"
#include "Utils.h"

#include <fstream>
#include <map>

void Makefile::Save(ConfigFile& conf, unsigned int flags)
{
  std::set<IncludeFile> hFiles; // hFile, directory
  std::set<std::string> cppFiles;
  if(flags & FLAG_SIMPLE)
    Utils::GetSourceFiles(conf, cppFiles);
  else
    Utils::GetSourceAndIncludeFiles(conf, hFiles, cppFiles);

  std::ofstream outputFile(conf.GetConfigPath()+ "Makefile");
  outputFile << "# This Makefile was generated using MakeGen "<< MAKEGEN_VERSION << " made by Tim Håkansson" << std::endl;
  outputFile << "# and is licensed under MIT. Full source of the project can be found at" << std::endl;
  outputFile << "# https://github.com/Thraix/MakeGen" << std::endl;
  outputFile << "CC=@g++" << std::endl;
  std::string outputtype = conf.GetSettingString(ConfigSetting::OutputType);
  if(outputtype != "executable")
  {
    if(outputtype == "sharedlibrary")
      outputFile << "CO=@g++ -shared -o" << std::endl;
    else
      outputFile << "CO=@g++ -o" << std::endl;
  }
  else
    outputFile << "CO=@g++ -o" << std::endl;

  outputFile << "MKDIR_P=mkdir -p" << std::endl;
  outputFile << "BIN=" << conf.GetSettingString(ConfigSetting::OutputDir) << std::endl;
  outputFile << "OBJPATH=$(BIN)intermediates" << std::endl;
  outputFile << "INCLUDES=";
  GetIncludePaths(outputFile, conf);
  outputFile << std::endl;
  outputFile << "OBJECTS=";
  for(auto it = cppFiles.begin();it!=cppFiles.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    size_t slash = it->find_last_of("/")+1;
    outputFile << "$(OBJPATH)/" << it->substr(slash, extensionPos - slash) << ".o ";
  }
  outputFile << std::endl;

  if(outputtype == "executable" || outputtype != "sharedlibrary")
    outputFile << "CFLAGS=$(INCLUDES) -std=c++17 -c ";
  else
    outputFile << "CFLAGS=$(INCLUDES) -fPIC -std=c++17 -c ";

  GetDefines(outputFile, conf);

  std::vector<std::string>& cflags = conf.GetSettingVectorString(ConfigSetting::CFlag);
  for(auto it = cflags.begin(); it != cflags.end(); ++it)
  {
    outputFile << *it << " ";
  }
  outputFile << std::endl;
  if(outputtype == "executable")
  {
    outputFile << "LIBDIRS=";
    GetLibraryPaths(outputFile, conf);
    outputFile << std::endl;

    outputFile << "LIBS=";
    GetLibraries(outputFile, conf);
    outputFile << std::endl;

    std::vector<std::string>& lflags = conf.GetSettingVectorString(ConfigSetting::LFlag);
    outputFile << "LDFLAGS=$(LIBDIRS) $(LIBS) ";
    for(auto it = lflags.begin(); it != lflags.end(); ++it)
    {
      outputFile << *it << " ";
    }
    outputFile << std::endl;

    std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
    if(!dependencies.empty())
    {
      outputFile << "DEPENDENCIES=";
      for(auto it = dependencies.begin();it!=dependencies.end();++it)
      {
        outputFile << *it << " ";
      }
      outputFile << std::endl;
    }
  }
  outputFile << "OUTPUT=$(BIN)" << conf.GetSettingString(ConfigSetting::OutputName) << std::endl;
  outputFile << ".PHONY: all directories rebuild clean run" << std::endl;

  // All
  outputFile << "all: directories $(OUTPUT)" << std::endl;

  // Directories
  outputFile << "directories: $(BIN) $(OBJPATH)" << std::endl;

  // Bin path
  outputFile << "$(BIN):" << std::endl;
  outputFile << "\t$(info Creating output directories)" << std::endl;
  outputFile << "\t@$(MKDIR_P) $(BIN)" << std::endl;

  // Object path
  outputFile << "$(OBJPATH):" << std::endl;
  outputFile << "\t@$(MKDIR_P) $(OBJPATH)" << std::endl;

  // Run
  outputFile << "run: all" << std::endl;
  if(outputtype == "executable")
  {
    std::vector<std::string>& prearguments = conf.GetSettingVectorString(ConfigSetting::ExecPreArgument);
    std::vector<std::string>& arguments = conf.GetSettingVectorString(ConfigSetting::ExecArgument);

    outputFile << "\t@";
    for(auto&& preargument : prearguments)
      outputFile << preargument << " ";
    outputFile << "./$(OUTPUT)";
    for(auto&& argument : arguments)
      outputFile << " " << argument;
    outputFile << std::endl;
  }

  // Rebuild
  outputFile << "rebuild: clean all" << std::endl;

  // Clean
  outputFile << "clean:" << std::endl;
  outputFile << "\t$(info Removing intermediates)" << std::endl;
  outputFile << "\trm -rf $(OBJPATH)/*.o" << std::endl;

  // Output file
  outputFile << "$(OUTPUT): $(OBJECTS)" << std::endl;
  outputFile << "\t$(info Generating output file)" << std::endl;
  if(outputtype == "executable")
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS) $(LDFLAGS)" << std::endl;
  else
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS)" << std::endl;

  // Install
  outputFile << "install: all" << std::endl;
  outputFile << "\t$(info Installing " << conf.GetSettingString(ConfigSetting::ProjectName) <<" to /usr/bin/)" << std::endl;
  outputFile << "\t@cp $(OUTPUT) /usr/bin/" << conf.GetSettingString(ConfigSetting::OutputName) << std::endl;

  std::map<std::string, IncludeDeps*> dependencies;
  size_t i = 0;
  for(auto it = cppFiles.begin(); it != cppFiles.end();++it)
  {
    i++;
    std::string& srcdir = conf.GetSettingString(ConfigSetting::SourceDir);
    auto itD = dependencies.find(srcdir + *it);
    if(itD == dependencies.end())
    {
      IncludeDeps* deps = new IncludeDeps(*it, conf.GetConfigPath() + srcdir,hFiles,dependencies);
      size_t extensionPos = it->find_last_of(".");
      size_t slash = it->find_last_of("/")+1;
      std::string oFile = it->substr(slash, extensionPos - slash)+".o ";

      outputFile << "$(OBJPATH)/" << oFile << ": ";
      deps->Output(outputFile, conf);
      outputFile << std::endl;
      outputFile << "\t$(info -[" << (int)(i / (float)cppFiles.size() * 100) << "%]- $<)" << std::endl;
      outputFile << "\t$(CC) $(CFLAGS) -o $@ $<" << std::endl;
    }
  }
}

void Makefile::GetIncludePaths(std::ostream& file, ConfigFile& conf, const std::string& confPath)
{
  std::vector<std::string>& includedirs = conf.GetSettingVectorString(ConfigSetting::IncludeDir);
  for(auto it = includedirs.begin(); it != includedirs.end(); ++it)
  {
    // Absolute path
    if((*it)[0] == '/')
      file << "-I " << *it << " ";
    else
      file << "-I " << confPath << *it << " ";
  }

  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(int i = 0; i < dependencies.size(); i++)
  {
    GetIncludePaths(file, conf.GetDependencyConfig(i), conf.GetDependencyConfig(i).GetConfigPath());
  }
}

void Makefile::GetLibraryPaths(std::ostream& file, ConfigFile& conf, const std::string& confPath)
{
  std::vector<std::string>& librarydirs = conf.GetSettingVectorString(ConfigSetting::LibraryDir);
  for(auto it = librarydirs.begin(); it != librarydirs.end(); ++it)
  {
    // Absolute path
    if((*it)[0] == '/')
      file << "-Wl,-rpath=" << *it << " " << "-L " << *it << " ";
    else
      file << "-Wl,-rpath=" << confPath << *it << " " << "-L " << confPath << *it << " ";
  }

  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(int i = 0; i < dependencies.size(); i++)
  {
    GetLibraryPaths(file, conf.GetDependencyConfig(i), conf.GetDependencyConfig(i).GetConfigPath());
  }
}

void Makefile::GetLibraries(std::ostream& file, ConfigFile& conf)
{
  std::vector<std::string>& libraries = conf.GetSettingVectorString(ConfigSetting::Library);
  for(auto it = libraries.begin(); it != libraries.end(); ++it)
  {
      file << "-l" << *it << " ";
  }

  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(int i = 0; i < dependencies.size(); i++)
  {
    GetLibraries(file, conf.GetDependencyConfig(i));
  }
}

void Makefile::GetDefines(std::ostream& file, ConfigFile& conf)
{
  std::vector<std::string>& defines = conf.GetSettingVectorString(ConfigSetting::Define);
  for(auto it = defines.begin(); it != defines.end(); ++it)
  {
    file << "-D " << *it << " ";
  }

  std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
  for(int i = 0; i < dependencies.size(); i++)
  {
    GetDefines(file, conf.GetDependencyConfig(i));
  }
}
