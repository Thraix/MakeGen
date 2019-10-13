#pragma once

#include <iostream>

#define BIT(x) (1<<x)

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

// Major changes, might not be backwards compatible
#define MAKEGEN_VERSION_MAJOR 1
// Release, should be backwards compatible
#define MAKEGEN_VERSION_RELEASE 2
// Minor changes, generally bug fixes
#define MAKEGEN_VERSION_MINOR 1

#define MAKEGEN_VERSION ("v" STR(MAKEGEN_VERSION_MAJOR) "." STR(MAKEGEN_VERSION_RELEASE) "." STR(MAKEGEN_VERSION_MINOR))

const static unsigned int FLAG_HELP = BIT(0);
const static unsigned int FLAG_VERSION = BIT(1);
const static unsigned int FLAG_CLEAN = BIT(2);
const static unsigned int FLAG_MAKE = BIT(3);
const static unsigned int FLAG_RUN = BIT(4);
const static unsigned int FLAG_INSTALL = BIT(5);
const static unsigned int FLAG_REBUILD = BIT(6);
const static unsigned int FLAG_SINGLE_THREAD = BIT(7);
const static unsigned int FLAG_DEPENDENCY = BIT(8);
const static unsigned int FLAG_SIMPLE = BIT(9);
const static unsigned int FLAG_CONFIG = BIT(10);


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
