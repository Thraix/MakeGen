#pragma once

#include "XMLObject.h"

#include <exception>
#include <string>

class XMLException : public std::exception
{
  private:
    std::string m_message;
  public:
    explicit XMLException(const std::string& message) : m_message("XMLException: " + message) {}
    explicit XMLException(const std::string& message, const XMLObject::XMLLoadData& data)
      : m_message("XMLException(" + data.file + ":" + std::to_string(data.line) + "): " + message)
    {}

    virtual const char* what() const throw()
    {
      return m_message.c_str();
    }
};
