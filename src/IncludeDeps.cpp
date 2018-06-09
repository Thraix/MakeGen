#include "IncludeDeps.h"

std::set<std::string> IncludeDeps::printSet;
int IncludeDeps::printCounter = 0;
IncludeDeps::IncludeDeps(const std::string& filename, const std::string& dir, const std::map<std::string, std::string>& files, std::map<std::string, IncludeDeps*>& allDeps)
      : filepath(dir+filename)
    {
      if(filename[filename.length() - 1] =='h')
      {
        allDeps.emplace(filepath, this);
      }
      std::ifstream file(filepath);
      std::string line;
      while(std::getline(file,line))
      {
        size_t pos = line.find("#include");
        if(pos != std::string::npos)
        {
          std::string include = GetIncludeFile(line, pos, filename);
          auto it = files.find(include);
          if(it != files.end())
          {
            auto itD = allDeps.find(it->second + it->first);
            if(itD == allDeps.end())
            {
              IncludeDeps* inc = new IncludeDeps(it->first, it->second,files,allDeps);
              dependencies.emplace(it->second+ it->first, inc);
            }else{
              dependencies.emplace(itD->first, itD->second);
            }
          }
        }
      }
    }

    std::string IncludeDeps::GetIncludeFile(const std::string& line, size_t pos, const std::string& filename)
    {
      size_t bracket = line.find('<',pos);
      if(bracket == std::string::npos)
      {
        bracket = line.find('\"',pos);
        if(bracket == std::string::npos)
        {
          return "";
        }
        size_t slash = filename.find_last_of("/");
        
        std::string include = line.substr(bracket+1, line.find('\"',bracket+1)-bracket-1);
        if(slash == std::string::npos)
          slash = -1;
        return filename.substr(0,slash+1)+include;
      }
      else
      {
        return line.substr(bracket+1, line.find('>',bracket+1)-bracket-1);
      }
    }
