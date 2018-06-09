#include "Makefile.h"
#include <map>
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include "Logging.h"
#include "IncludeDeps.h"

void Makefile::GetAllFiles(const std::string& folder, std::vector<std::string>& files)
{
  DIR* dp;
  struct dirent *dirp;
  if((dp = opendir(folder.c_str())) == NULL){
    LOG_ERROR(errno);
    return;
  }
  while((dirp = readdir(dp)) != NULL)
  {
    if(dirp->d_type == DT_DIR)
    {
      if(strcmp(dirp->d_name,".") == 0)
        continue;
      if(strcmp(dirp->d_name,"..") == 0)
        continue;
      GetAllFiles(folder+dirp->d_name+"/", files);
    }
    else
    {
      files.push_back(folder+dirp->d_name);
    }
  }
  closedir(dp);
}

void Makefile::Save(const ConfigFile& conf)
{
  std::map<std::string, std::string> hFiles;
  std::map<std::string, std::string> cppFiles;
  PreSave(conf,hFiles,cppFiles);

  std::ofstream outputFile("Makefile");
  outputFile << "CC=@g++" << std::endl;
  if(!conf.executable)
    outputFile << "CO=@ar rs" << std::endl;
  else
    outputFile << "CO=@g++ -o" << std::endl;

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
    size_t extensionPos = it->first.find_last_of(".");
    size_t slash = it->first.find_last_of("/")+1;
    outputFile << "$(OBJPATH)/" << it->first.substr(slash, extensionPos - slash) << ".o ";
  }
  outputFile << std::endl;
  outputFile << "CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 ";
  for(auto it = conf.defines.begin();it!=conf.defines.end();++it)
  {
    outputFile << "-D" << *it << " ";
  }
  outputFile << std::endl;
  outputFile << "LIBS=";
  for(auto it = conf.libs.begin();it!=conf.libs.end();++it)
  {
    outputFile << "-l:" << *it << " ";
  }
  outputFile << std::endl;
  outputFile << "OUTPUT=$(BIN)" << conf.outputname << std::endl;
  outputFile << "all: $(OUTPUT)" << std::endl;
  outputFile << "\t$(info ------------------------)" << std::endl;
  outputFile << "\t$(info ---- Done Compiling ----)" << std::endl;
  outputFile << "\t$(info ------------------------)" << std::endl;
  outputFile << "rebuid: clean all" << std::endl;
  outputFile << "clean:" << std::endl;
  outputFile << "\t$(info Removing intermediates)" << std::endl;
  outputFile << "\trm -rf $(OBJPATH)/*.o" << std::endl;
  outputFile << "$(OUTPUT): $(OBJECTS)" << std::endl;
  outputFile << "\t$(info Generating output file)" << std::endl;
  outputFile << "\t$(CO) $(OUTPUT) $(OBJECTS) $(LIBS)" << std::endl;
  outputFile << "install: all" << std::endl;
  outputFile << "\tcp $(OUTPUT) /usr/bin/" << conf.outputname << std::endl;
  std::map<std::string, IncludeDeps*> dependencies;
  for(auto it = cppFiles.begin(); it!=cppFiles.end();++it)
  {
    auto itD = dependencies.find(it->first+it->second);
    if(itD == dependencies.end())
    {
      IncludeDeps* deps = new IncludeDeps(it->first, it->second,hFiles,dependencies);
      size_t extensionPos = it->first.find_last_of(".");
      size_t slash = it->first.find_last_of("/")+1;
      std::string oFile = it->first.substr(slash, extensionPos - slash)+".o ";
      outputFile << "$(OBJPATH)/" << oFile << ": " << *deps << std::endl;
      outputFile << "\t$(info ---- $<)" << std::endl;
      outputFile << "\t$(CC) $(CFLAGS) -o $@ $<" << std::endl;
      //std::cout << *deps << std::endl;
    }
  }
}

void Makefile::PreSave(const ConfigFile& conf, std::map<std::string, std::string>& hFiles, 
    std::map<std::string, std::string>& cppFiles)
{
  for(auto itSrc = conf.srcdirs.begin();itSrc != conf.srcdirs.end();++itSrc)
  {
    std::vector<std::string> files;
    GetAllFiles(*itSrc,files);
    // include paramenter with the path of the file
    // For example src/graphics/Window.h -> graphics/Window.h if src is a src folder 
    for(auto it = files.begin(); it!=files.end();++it)
    {
      size_t extensionPos = it->find_last_of(".");
      if(extensionPos != std::string::npos)
      {
        if(it->substr(extensionPos+1) == "cpp")
        {
          cppFiles.emplace(it->substr(itSrc->length()), *itSrc);
        }
        else
        {
          hFiles.emplace(it->substr(itSrc->length()), *itSrc);
        }
      }
    }
  }
  
}
