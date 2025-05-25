#include "IncludeDeps.h"

#include "Common.h"
#include <sstream>

std::set<std::string> IncludeDeps::printSet;
int IncludeDeps::printCounter = 0;

IncludeDeps::IncludeDeps(const std::string& filename, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps)
  : IncludeDeps{filename, false, files, allDeps}
{}

IncludeDeps::IncludeDeps(const std::string& filename, bool projectHFile, const std::set<HFile>& files, std::map<std::string, IncludeDeps*>& allDeps)
  : filepath(filename), projectHFile{projectHFile}
{
  std::filesystem::path path{filepath};
  if(Utils::IsHeaderFile(filename))
  {
    allDeps.emplace(filepath, this);
  }
  std::ifstream file(filepath);
  std::string line;
  while(std::getline(file,line))
  {
    std::string start;
    std::stringstream ss{line};
    ss >> start;
    if(start == "#include")
    {
      std::string include = GetIncludeFile(line);

      // Check if file can be found relative to current file:
      std::filesystem::path includeFileRelativeToSource = std::filesystem::relative(path.parent_path(), ".").string() + "/" + include;
      if (FileUtils::FileExists(includeFileRelativeToSource.string()))
      {
        auto itD = allDeps.find(includeFileRelativeToSource.string());
        if(itD == allDeps.end())
        {
          IncludeDeps* inc = new IncludeDeps(includeFileRelativeToSource.string(), false, files, allDeps);
          dependencies.emplace(includeFileRelativeToSource, inc);
        } else {
          dependencies.emplace(itD->first, itD->second);
        }
      }
      else
      {
        auto it = files.find({include, "", false});
        if(it != files.end())
        {
          auto itD = allDeps.find(it->filepath);
          if(itD == allDeps.end())
          {
            IncludeDeps* inc = new IncludeDeps(it->filepath, it->isProjectHFile, files, allDeps);
            dependencies.emplace(it->filepath, inc);
          }else{
            dependencies.emplace(itD->first, itD->second);
          }
        }
      }
    }
  }
}

std::string IncludeDeps::GetIncludeFile(const std::string& line)
{
  size_t bracket = line.find('<');
  if(bracket == std::string::npos)
  {
    bracket = line.find('\"');
    if(bracket == std::string::npos)
    {
      return "";
    }

    std::string include = line.substr(bracket + 1, line.find('\"',bracket+1)-bracket-1);
    return include;
  }
  else
  {
    return line.substr(bracket+1, line.find('>',bracket+1)-bracket-1);
  }
}
