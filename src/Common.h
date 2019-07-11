#pragma once

#include <iostream>
#include <string>

#define BIT(x) (1<<x)

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

// Major changes, probably not be backwards compatible
#define MAKEGEN_VERSION_MAJOR 1
// Release , should be backwards compatible with any minor version
#define MAKEGEN_VERSION_RELEASE 1
// Minor changes, should be compatible with any other minor version with same major and release.
#define MAKEGEN_VERSION_MINOR 3
#define MAKEGEN_VERSION ("v" STR(MAKEGEN_VERSION_MAJOR) "." STR(MAKEGEN_VERSION_RELEASE) "." STR(MAKEGEN_VERSION_MINOR))

const static unsigned int FLAG_HELP = BIT(0);
const static unsigned int FLAG_GEN = BIT(1);
const static unsigned int FLAG_VERSION = BIT(2);


#define LOG_INFO(...) Log(__VA_ARGS__); std::cout << std::endl
#define LOG_WARNING(...) Log(__VA_ARGS__); std::cout << std::endl
#define LOG_ERROR(...) Log(__VA_ARGS__); std::cout << std::endl

template <typename T>
void Log(const T& var)
{
  std::cout << var;
}

template <typename T, typename ...Ts>
void Log(const T& var, const Ts& ...vars)
{
  Log(var);
  Log(vars...);
}

inline std::string CommonPrefix(const std::string& s1, const std::string& s2)
{
  size_t n = 0;
  for(size_t i = 0; i<s1.size() && i<s2.size();++i)
  {
    if(s1[i] != s2[i])
    {
      n = i;
      break;
    }
  }
  return s1.substr(0, n);
}
