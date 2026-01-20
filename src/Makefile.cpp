#include "Makefile.h"

#include <fstream>
#include <map>

#include "Common.h"
#include "IncludeDeps.h"
#include "Utils.h"

void Makefile::Save(ConfigFile& conf, unsigned int flags)
{
  std::set<HFile> hFiles;  // hFile, directory
  std::set<std::string> cppFiles;
  if (flags & FLAG_SIMPLE)
    Utils::GetCppFiles(conf, cppFiles);
  else
    Utils::GetCppAndHFiles(conf, hFiles, cppFiles);

  std::ofstream outputFile(conf.GetConfigPath() + "Makefile");
  outputFile << "# This Makefile was generated using MakeGen " << MAKEGEN_VERSION << " made by Tim Håkansson"
             << std::endl;
  outputFile << "# and is licensed under MIT. Full source of the project can be found at" << std::endl;
  outputFile << "# https://github.com/Thraix/MakeGen" << std::endl;
  outputFile << "CC=@g++" << std::endl;
  std::string outputtype = conf.GetSettingString(ConfigSetting::OutputType);
  if (outputtype != "executable")
  {
    if (outputtype == "sharedlibrary")
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
  std::vector<std::string>& includedirs = conf.GetSettingVectorString(ConfigSetting::IncludeDir);
  for (auto it = includedirs.begin(); it != includedirs.end(); ++it)
  {
    outputFile << "-I " << *it << " ";
  }

  std::vector<std::string>& includedirsexcldep = conf.GetSettingVectorString(ConfigSetting::IncludeDirExclDep);
  for (auto it = includedirsexcldep.begin(); it != includedirsexcldep.end(); ++it)
  {
    outputFile << "-I " << *it << " ";
  }
  outputFile << std::endl;
  outputFile << "OBJECTS=";
  for (auto it = cppFiles.begin(); it != cppFiles.end(); ++it)
  {
    std::filesystem::path cppFile(*it);
    std::filesystem::path oFile = cppFile.replace_extension("o");
    std::string oFileStr = oFile.string();
    Utils::Replace(oFileStr, "..", "dotdot");
    outputFile << "$(OBJPATH)/" << oFileStr << " ";
  }
  outputFile << std::endl;
  if (outputtype == "executable" || outputtype != "sharedlibrary")
  {
    outputFile << "CFLAGS=$(INCLUDES) -std=c++17 -c ";
  }
  else
  {
    outputFile << "CFLAGS=$(INCLUDES) -fPIC -std=c++17 -c ";
  }
  std::vector<std::string>& defines = conf.GetSettingVectorString(ConfigSetting::Define);
  for (auto it = defines.begin(); it != defines.end(); ++it)
  {
    outputFile << "-D" << *it << " ";
  }
  std::vector<std::string>& cflags = conf.GetSettingVectorString(ConfigSetting::CFlag);
  for (auto it = cflags.begin(); it != cflags.end(); ++it)
  {
    outputFile << *it << " ";
  }
  outputFile << std::endl;
  if (outputtype == "executable")
  {
    std::vector<std::string>& libdirs = conf.GetSettingVectorString(ConfigSetting::LibraryDir);
    outputFile << "LIBDIR=";
    for (auto it = libdirs.begin(); it != libdirs.end(); ++it)
    {
      outputFile << "-L " << *it << " ";
    }
    outputFile << std::endl;
    std::vector<std::string>& lflags = conf.GetSettingVectorString(ConfigSetting::LFlag);
    outputFile << "LDFLAGS=";
    for (auto it = lflags.begin(); it != lflags.end(); ++it)
    {
      outputFile << *it << " ";
    }
    for (auto it = libdirs.begin(); it != libdirs.end(); ++it)
    {
      outputFile << "-Wl,-rpath=" << *it << " ";
    }
    outputFile << std::endl;
    std::vector<std::string>& libs = conf.GetSettingVectorString(ConfigSetting::Library);
    outputFile << "LIBS=$(LIBDIR) ";
    for (auto it = libs.begin(); it != libs.end(); ++it)
    {
      outputFile << "-l" << *it << " ";
    }
    outputFile << std::endl;
    std::vector<std::string>& dependencies = conf.GetSettingVectorString(ConfigSetting::Dependency);
    if (!dependencies.empty())
    {
      outputFile << "DEPENDENCIES=";
      for (auto it = dependencies.begin(); it != dependencies.end(); ++it)
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
  std::set<std::string> intermediateDirectories = GetIntermediateDirectories(cppFiles);
  outputFile << "directories: ";
  for (const auto& intermediateDirectory : intermediateDirectories)
  {
    outputFile << intermediateDirectory << " ";
  }
  outputFile << std::endl;

  // Intermediate directories
  for (const auto& intermediateDirectory : intermediateDirectories)
  {
    outputFile << intermediateDirectory << ":" << std::endl;
    outputFile << "\t@$(MKDIR_P) $@" << std::endl;
  }

  // Run
  outputFile << "run: all" << std::endl;
  if (outputtype == "executable")
  {
    std::vector<std::string>& prearguments = conf.GetSettingVectorString(ConfigSetting::ExecPreArgument);
    std::vector<std::string>& arguments = conf.GetSettingVectorString(ConfigSetting::ExecArgument);

    outputFile << "\t@";
    for (auto&& preargument : prearguments) outputFile << preargument << " ";
    outputFile << "./$(OUTPUT)";
    for (auto&& argument : arguments) outputFile << " " << argument;
    outputFile << std::endl;
  }

  // Rebuild
  outputFile << "rebuild: clean all" << std::endl;

  // Clean
  outputFile << "clean:" << std::endl;
  outputFile << "\t$(info Removing $(OBJPATH))" << std::endl;
  outputFile << "\t@rm -rf $(OBJPATH)/" << std::endl;

  // Output file
  outputFile << "$(OUTPUT): $(OBJECTS)" << std::endl;
  outputFile << "\t$(info Generating output file $(OUTPUT))" << std::endl;
  if (outputtype == "executable")
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS) $(LDFLAGS) $(LIBS)" << std::endl;
  else
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS)" << std::endl;

  // Install
  outputFile << "install: all" << std::endl;
  outputFile << "\t$(info Installing " << conf.GetSettingString(ConfigSetting::ProjectName) << " to /usr/bin/)"
             << std::endl;
  outputFile << "\t@cp $(OUTPUT) /usr/bin/" << conf.GetSettingString(ConfigSetting::OutputName) << std::endl;

  std::map<std::string, IncludeDeps*> dependencies;
  size_t i = 0;
  for (auto it = cppFiles.begin(); it != cppFiles.end(); ++it)
  {
    i++;
    auto itD = dependencies.find(*it);
    if (itD == dependencies.end())
    {
      std::filesystem::path cppFile(*it);
      std::filesystem::path oFile = cppFile.replace_extension("o");
      std::string oFileStr = oFile.string();
      Utils::Replace(oFileStr, "..", "dotdot");
      outputFile << "$(OBJPATH)/" << oFileStr << ":";
      if (flags & FLAG_SIMPLE)
      {
        outputFile << " " << *it;
      }
      else
      {
        IncludeDeps* deps = new IncludeDeps(*it, hFiles, dependencies);
        deps->Output(outputFile, conf);
      }
      outputFile << std::endl;
      outputFile << "\t$(info -[" << (int)(i / (float)cppFiles.size() * 100) << "%]- $<)" << std::endl;
      outputFile << "\t$(CC) $(CFLAGS) -o $@ $<" << std::endl;
    }
  }
}

std::set<std::string> Makefile::GetIntermediateDirectories(const std::set<std::string>& cppFiles)
{
  std::set<std::string> intermediateDirectories;
  for (const auto& cppFile : cppFiles)
  {
    std::filesystem::path cppPath{cppFile};
    std::filesystem::path oFile = cppPath.replace_extension("o");
    std::string parentPathStr = oFile.parent_path().string();
    Utils::Replace(parentPathStr, "..", "dotdot");
    intermediateDirectories.emplace("$(OBJPATH)/" + parentPathStr);
  }
  return intermediateDirectories;
}
