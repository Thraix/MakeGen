#include "IncludeDeps.h"

#include "Common.h"

std::set<std::string> IncludeDeps::printSet;
int IncludeDeps::printCounter = 0;

IncludeDeps::IncludeDeps(const std::string& filename, const std::string& dir, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps)
  : IncludeDeps{filename, dir, false, files, allDeps}
{}

IncludeDeps::IncludeDeps(const std::string& filename, const std::string& dir, bool projectHFile, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps)
  : filepath(dir+filename), projectHFile{projectHFile}
{
  if(Utils::IsHeaderFile(filename))
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
      std::string include = FileUtils::CollapseDirectory(GetIncludeFile(line, pos, filename));
      auto it = files.find({include, "", false});
      if(it != files.end())
      {
        auto itD = allDeps.find(it->filepath);
        if(itD == allDeps.end())
        {
          IncludeDeps* inc = new IncludeDeps(it->filename,it->directory, it->isProjectHFile, files,allDeps);
          dependencies.emplace(it->filepath, inc);
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
