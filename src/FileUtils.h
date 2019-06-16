#pragma once

#include <string>
#include <fstream>
#include <dirent.h>
#include "Common.h"
#include <cstring>
#include <vector>
#include <stdlib.h>

struct FileUtils
{
  static std::string GetRealPath(const std::string& filename)
  {
#if defined(__linux__)
    char* path = realpath(filename.c_str(), NULL);
    std::string sPath = path;
    sPath+="/";
    free(path);
    return sPath;
#endif
    LOG_ERROR("GetRealPath not supported");
    return filename;
  }

  static void GetAllFiles(const std::string& folder, std::vector<std::string>& files)
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
};
