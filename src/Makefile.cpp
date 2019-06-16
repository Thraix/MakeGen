#include "Makefile.h"
#include <map>
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include "IncludeDeps.h"
#include "Common.h"
#include "FileUtils.h"

void Makefile::Save(const ConfigFile& conf)
{
  std::map<std::string, std::string> hFiles; // hFile, directory
  std::set<std::string> cppFiles;
  GetCppAndHFiles(conf,hFiles,cppFiles);

  std::ofstream outputFile(conf.configPath + "Makefile");
  outputFile << "# This Makefile was generated using MakeGen "<< MAKEGEN_VERSION<< " made by Tim Håkansson" << std::endl;
  outputFile << "# and is licensed under MIT. Full source of the project can be found at" << std::endl;
  outputFile << "# https://github.com/Thraix/MakeGen" << std::endl;
  outputFile << "CC=@g++" << std::endl;
  if(!conf.executable)
  {
    if(conf.shared)
      outputFile << "CO=@g++ -shared -o" << std::endl;
    else
      outputFile << "CO=@g++ -o" << std::endl;
  }
  else
    outputFile << "CO=@g++ -o" << std::endl;

  outputFile << "MKDIR_P=mkdir -p" << std::endl;
  outputFile << "BIN=" << conf.outputdir << std::endl;
  outputFile << "OBJPATH=$(BIN)intermediates" << std::endl;
  outputFile << "INCLUDES=";
  for(auto it = conf.includedirs.begin();it!=conf.includedirs.end();++it)
  {
    outputFile << "-I./" << *it << " ";
  }
  outputFile << std::endl;
  outputFile << "OBJECTS=";
  for(auto it = cppFiles.begin();it!=cppFiles.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    size_t slash = it->find_last_of("/")+1;
    outputFile << "$(OBJPATH)/" << it->substr(slash, extensionPos - slash) << ".o ";
  }
  outputFile << std::endl;
  if(conf.executable || !conf.shared)
  {
    outputFile << "CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 ";
  }
  else
  {
    outputFile << "CFLAGS=$(INCLUDES) -fPIC -std=c++17 -c -w -g3 ";
  }
  for(auto it = conf.defines.begin();it!=conf.defines.end();++it)
  {
    outputFile << "-D" << *it << " ";
  }
  for(auto it = conf.flags.begin();it!=conf.flags.end();++it)
  {
    outputFile << *it << " ";
  }
  outputFile << std::endl;
  if(conf.executable)
  {
    outputFile << "LIBDIR=";
    for(auto it = conf.libdirs.begin();it!=conf.libdirs.end();++it)
    {
      outputFile << "-L./" << *it << " ";
    }
    outputFile << std::endl;
    outputFile << "LDFLAGS=";
    for(auto it = conf.libdirs.begin();it!=conf.libdirs.end();++it)
    {
      outputFile << "-Wl,-rpath=" << *it << " ";
    }
    outputFile << std::endl;
    outputFile << "LIBS=$(LIBDIR) ";
    for(auto it = conf.libs.begin();it!=conf.libs.end();++it)
    {
      outputFile << "-l" << *it << " ";
    }
    outputFile << std::endl;
    if(!conf.dependencies.empty())
    {
      outputFile << "DEPENDENCIES=";
      for(auto it = conf.dependencies.begin();it!=conf.dependencies.end();++it)
      {
        outputFile << *it << " ";
      }
      outputFile << std::endl;
    }
  }
  outputFile << "OUTPUT=$(BIN)" << conf.outputname << std::endl;
  outputFile << ".PHONY: all directories rebuild clean dependencies" << std::endl;
  outputFile << "all: dependencies directories $(OUTPUT)" << std::endl;
  //outputFile << "\t$(info ------------------------)" << std::endl;
  //outputFile << "\t$(info ---- Done Compiling ----)" << std::endl;
  //outputFile << "\t$(info ------------------------)" << std::endl;

  outputFile << "dependencies:" << std::endl;
  if(!conf.dependencies.empty())
  {
    //outputFile << "\t$(info Building dependencies)" << std::endl;
    //outputFile << "\t@for dep in $(DEPENDENCIES); do\\" << std::endl;
    //outputFile << "\t\tmakegen -C $$dep;\\" << std::endl;
    //outputFile << "\tdone" << std::endl;
  }
  outputFile << "directories: $(BIN) $(OBJPATH)" << std::endl;
  outputFile << "$(BIN):" << std::endl;
  outputFile << "\t$(info Creating output directories)" << std::endl;
  outputFile << "\t@$(MKDIR_P) $(BIN)" << std::endl;
  outputFile << "$(OBJPATH):" << std::endl;
  outputFile << "\t@$(MKDIR_P) $(OBJPATH)" << std::endl;
  outputFile << "rebuild: clean all" << std::endl;
  outputFile << "clean:" << std::endl;
  outputFile << "\t$(info Removing intermediates)" << std::endl;
  outputFile << "\trm -rf $(OBJPATH)/*.o" << std::endl;
  outputFile << "$(OUTPUT): $(OBJECTS)" << std::endl;
  outputFile << "\t$(info Generating output file)" << std::endl;
  if(conf.executable)
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS) $(LDFLAGS) $(LIBS)" << std::endl;
  else
    outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS)" << std::endl;
  outputFile << "install: all" << std::endl;
  outputFile << "\t$(info Installing " << conf.projectname <<" to /usr/bin/)" << std::endl;
  outputFile << "\t@cp $(OUTPUT) /usr/bin/" << conf.outputname << std::endl;
  std::map<std::string, IncludeDeps*> dependencies;
  size_t i = 0;
  for(auto it = cppFiles.begin(); it!=cppFiles.end();++it)
  {
    i++;
    auto itD = dependencies.find(conf.srcdir + *it);
    if(itD == dependencies.end())
    {
      IncludeDeps* deps = new IncludeDeps(*it, conf.configPath+conf.srcdir,hFiles,dependencies);
      size_t extensionPos = it->find_last_of(".");
      size_t slash = it->find_last_of("/")+1;
      std::string oFile = it->substr(slash, extensionPos - slash)+".o ";
      outputFile << "$(OBJPATH)/" << oFile << ": " << *deps << std::endl;
      outputFile << "\t$(info -[" << (int)(i / (float)cppFiles.size() * 100) << "%]- $<)" << std::endl;
      outputFile << "\t$(CC) $(CFLAGS) -o $@ $<" << std::endl;
      //std::cout << *deps << std::endl;
    }
  }
}

void Makefile::GetCppAndHFiles(const ConfigFile& conf, std::map<std::string, std::string>& hFiles, std::set<std::string>& cppFiles)
{
  std::vector<std::string> files;
  std::string path = conf.configPath + conf.srcdir;
  FileUtils::GetAllFiles(path,files);
  // include paramenter with the path of the file
  // For example src/graphics/Window.h -> graphics/Window.h if src is a src folder 
  for(auto it = files.begin(); it!=files.end();++it)
  {
    size_t extensionPos = it->find_last_of(".");
    if(extensionPos != std::string::npos)
    {
      std::string extension = it->substr(extensionPos+1);
      std::string filename = it->substr(path.length());
      if(extension == "cpp" || extension == "c")
      {
        cppFiles.emplace(filename);
      }
      else if(extension == "hpp" || extension == "h")
      {
        hFiles.emplace(filename,path);
      }
    }
  }
  for(size_t i = 0; i < conf.dependencies.size(); ++i)
  {
    std::vector<std::string> files;
    std::string depSrcDir = conf.dependencies[i] + conf.dependencyConfigs[i].srcdir;
    FileUtils::GetAllFiles(depSrcDir, files);
    for(auto it = files.begin(); it!=files.end();++it)
    {
      size_t extensionPos = it->find_last_of(".");
      if(extensionPos != std::string::npos)
      {
        std::string extension = it->substr(extensionPos+1);
        if(extension == "hpp" || extension == "h")
        {
          hFiles.emplace(it->substr(depSrcDir.length()),depSrcDir);
        }
      }
    }
  }
}
