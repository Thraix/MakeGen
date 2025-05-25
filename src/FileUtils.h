#pragma once

#include "Common.h"
#include "Utils.h"
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <filesystem>

struct FileUtils
{
  static bool HasPath(const std::string& path)
  {
    struct stat info;

    if(stat(path.c_str(), &info) != 0)
      return false;
    else
      return true;
  }

  static bool CreateDirectory(const std::string& path)
  {
    return mkdir(path.c_str(), 0777);
  }

  static std::string GetCurrentDirectory()
  {
    static char path[256]; // Usual maximum filename
    getcwd(path, sizeof(path));
    return GetTopDirectory(path);
  }

  static std::string GetTopDirectory(const std::string& dir)
  {
    if(dir.size() == 0)
    {
      LOG_ERROR("Cannot send empty string to FileUtils::GetTopDirectory()");
      assert(false);
    }
    size_t dirEnd = std::string::npos;
    if(dir[dir.size()-1] == '/')
      dirEnd = dir.size()-2;
    size_t pos = dir.find_last_of("/", dirEnd);
    if(pos == std::string::npos)
    {
      LOG_ERROR("Couldn't find / (slash) in directory. This shouldn't occur.");
      assert(false);
    }
    return dir.substr(pos + 1, dirEnd - pos);
  }

  static std::string GetRealPath(const std::string& filename)
  {
#if defined(__linux__)
    if(access(filename.c_str(), F_OK ) != -1)
    {
      char* path = realpath(filename.c_str(), NULL);
      std::string sPath = path;
      sPath+="/";
      free(path);
      return sPath;
    }
    else
    {
      LOG_ERROR("Directory doesn't exist: ", filename);
      return "";
    }
#endif
    LOG_ERROR("GetRealPath not supported");
    return "";
  }

  static std::string GetRelativePath(std::string from, std::string to)
  {
    std::string result;
    if(to[to.size()-1] == '/')
      to.pop_back();
    if(from[from.size()-1] == '/')
      from.pop_back();

    // Check if the directory is inside 'from'
    if(strncmp(to.c_str(), from.c_str(), from.size()) == 0)
    {
      // Same directory
      if(to.size() == from.size())
        return "";
      // Remove the 'from' path
      return to.substr(from.size()+1);
    }
    // Check if the directory is a child of from
    else if(strncmp(from.c_str(), to.c_str(), to.size()) == 0)
    {
      std::string sub = from.substr(to.size());
      size_t n = std::count(sub.begin(), sub.end(), '/');
      for(int i = 0;i<n;i++)
      {
        result+="..";
        if(i != n-1)
          result+="/";
      }
      return result;
    }
    // Otherwise it's a path in another directory
    else
    {
      // Find the most common directory
      std::string commonPath = Utils::CommonPrefix(from,to);
      if (commonPath.empty())
        return "";
      while(commonPath.back() != '/')
        commonPath.pop_back();
      commonPath.pop_back();

      // Go back to the common directory
      std::string sub = from.substr(commonPath.size());
      size_t n = std::count(sub.begin(), sub.end(), '/');
      for(int i = 0;i<n;i++)
      {
        result+="..";
        if(i != n-1)
          result+="/";
      }
      // Add the path which diverges
      result += to.substr(commonPath.size());
      return result;
    }
  }

  static void GetAllFiles(const std::string& folder, std::vector<std::string>& files)
  {
    DIR* dp;
    struct dirent *dirp;
    if((dp = opendir(folder.c_str())) == NULL){
      LOG_ERROR("Failed to open directory: ", folder);
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

  static bool FileExists(const std::string& filename)
  {
    return std::filesystem::exists(filename) && !std::filesystem::is_directory(filename);
  }
};
