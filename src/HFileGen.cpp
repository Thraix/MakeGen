#include "HFileGen.h"

#include "FileUtils.h"
#include <set>

void HFileGen::Create(const ConfigFile& conf)
{
  std::set<std::string> hFiles;
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
      std::string filename = it->substr(path.length());
      if(it->substr(extensionPos+1) == "h" && filename != conf.configPath + conf.hFile)
      {
        // Make files sorted in alphabetical order
        hFiles.emplace(filename);
      }
    }
  }

  std::ofstream os(conf.configPath + conf.srcdir+"/"+conf.hFile);
  os << "#pragma once" << std::endl << std::endl;
  for(auto&& hFile : hFiles)
    os << "#include <" << hFile << ">" << std::endl; 
}
