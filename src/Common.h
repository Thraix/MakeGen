#pragma once

#include <iostream>

#define BIT(x) (1<<x)

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

#define MAKEGEN_VERSION_MAJOR 1
#define MAKEGEN_VERSION_RELEASE 0
#define MAKEGEN_VERSION_MINOR 4
#define MAKEGEN_VERSION ("v" STR(MAKEGEN_VERSION_MAJOR) "." STR(MAKEGEN_VERSION_RELEASE) "." STR(MAKEGEN_VERSION_MINOR))

const static unsigned int FLAG_HELP = BIT(0);
const static unsigned int FLAG_GEN= BIT(1);
const static unsigned int FLAG_VERSION= BIT(2);


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

