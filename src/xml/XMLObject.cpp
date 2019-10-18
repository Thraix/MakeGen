#include "XMLObject.h"

#include "../Common.h"
#include "../Utils.h"

#include <cstring>
#include "XMLException.h"

XMLObject::XMLObject(const std::string& string)
{
  int pos = 0;
  int line = 1;
  XMLLoadData data{pos, line, ""};
  if(!ReadHead(string, data))
    ReadBodyTail(string, data);
}

XMLObject::XMLObject(const std::string& string, int pos, int line, const std::string& file)
{
  XMLLoadData data{pos, line, file};
  if (!ReadHead(string, data))
    ReadBodyTail(string, data);
}

XMLObject::XMLObject(const std::string& string, XMLLoadData& data)
{
  if (!ReadHead(string, data))
    ReadBodyTail(string, data);
}

XMLObject::XMLObject(const std::string& name, const std::map<std::string, std::string>& attributes, const std::string& text)
  :name(name), attributes(attributes), text(text)
{

}

XMLObject::XMLObject(const std::string& name, const std::map<std::string, std::string>& attributes, const std::map<std::string,std::vector<XMLObject>>& objects)
  : name(name), attributes(attributes), objects(objects)
{

}

bool XMLObject::HasAttribute(const std::string& property) const
{
  return attributes.find(property) != attributes.end();
}

const std::string& XMLObject::GetAttribute(const std::string& property) const
{
  auto it = attributes.find(property);
  if (it == attributes.end())
    throw XMLException((std::string("Attribute could not be found \"") + property + "\".").c_str());
  return it->second;
}

const std::string& XMLObject::GetAttribute(const std::string& property, const std::string& defaultValue) const
{
  auto it = attributes.find(property);
  if (it == attributes.end())
    return defaultValue;
  return it->second;
}

unsigned int XMLObject::GetObjectCount() const
{
  return objects.size();
}

std::vector<XMLObject>* XMLObject::GetObjectPtr(const std::string& name)
{
  auto it = objects.find(name);
  if(it == objects.end())
    return nullptr;

  return &it->second;
}

const std::map<std::string, std::vector<XMLObject>>& XMLObject::GetObjects() const
{
  return objects;
}

const std::string& XMLObject::GetName() const
{
  return name;
}

const std::string& XMLObject::GetText() const
{
  return text;
}

void XMLObject::SetName(const std::string& name)
{
  if(Utils::IsWord(name))
    this->name = name;
  else
    LOG_ERROR("XML Head can only be made up of letters");
}

void XMLObject::SetText(const std::string& text)
{
  this->text = text;
}

void XMLObject::AddAttribute(const std::string& property, const std::string& value)
{
  if(Utils::IsWord(property))
    attributes.emplace(property, value);
  else
    LOG_ERROR("XML property name can only be made up of letters");
}

void XMLObject::AddXMLObject(const XMLObject& object)
{
  auto it = objects.find(object.name);
  if(it == objects.end())
    objects.emplace(object.name, std::vector<XMLObject>{object});
  else
    it->second.push_back(object);
}

bool XMLObject::RemoveXMLObject(const XMLObject& object)
{
  auto it = objects.find(object.name);
  if(it == objects.end())
    return false;

  bool removed = false;
  for(auto it2 = it->second.begin(); it2 != it->second.end();)
  {
    if(*it2 == object)
    {
      it2 = it->second.erase(it2);
      removed = true;
    }
    else 
      ++it2;
  }
  return removed;
}

XMLObject XMLObject::GetStrippedXMLObject() const
{
  if(text == "")
    return XMLObject(name, attributes, objects);
  else
    return XMLObject(name, attributes, text);
}

////////////////////////////////////////////////////////////
//                                                        //
// Everything below here handles the reading of xml files //
//                                                        //
////////////////////////////////////////////////////////////


bool XMLObject::ReadHead(const std::string& string, XMLLoadData& data)
{
  // Check if the first character is the start of and xml tag.
  ReadWhiteSpace(string, data);
  if (string[data.pos] != '<')
    throw XMLException("Not an XML Object.", data);

  // Check if there is a closing tag
  size_t closing = string.find('>');
  if (closing == std::string::npos)
    throw XMLException("No enclosing > for opening tag.", data);

  // Read the name of the tag
  ReadName(string, data);

  //  Read all attributes of the xml tag
  ReadAttributes(string, data);

  // Read opening tag
  if (string[data.pos] == '/')
  {
    data.pos++;
    ReadWhiteSpace(string, data);
    if (string[data.pos] != '>')
      throw XMLException((std::string("Invalid character proceeding / in opening XML Tag \"") + string[data.pos] + "\".").c_str(), data);
    data.pos++;
    // nothing more to read.
    return true;
  }

  ReadWhiteSpace(string, data);
  if (string[data.pos] != '>')
    throw XMLException((std::string("Invalid character proceeding attributes in opening XML Tag \"") + string[data.pos] + "\".").c_str(), data);
  (data.pos)++;
  return false;
}

void XMLObject::ReadName(const std::string& string, XMLLoadData& data)
{
  data.pos++;
  ReadWhiteSpace(string, data);
  if (!Utils::IsLetter(string[data.pos]))
    throw XMLException("Invalid XML name. Can only contain letters.", data);
  name = Utils::GetWord(string, data.pos);
  data.pos += name.length();
  ReadWhiteSpace(string, data);
  if (string[data.pos] != '/' && string[data.pos] != '>' && Utils::IsWhiteSpace(string[data.pos]))
  {
    throw XMLException((std::string("Invalid character proceeding name in XML Tag \"") + string[data.pos] + "\".").c_str(), data);
  }
}

void XMLObject::ReadAttributes(const std::string& string, XMLLoadData& data)
{
  ReadWhiteSpace(string, data);

  while (string[data.pos] != '>' && string[data.pos] != '/')
  {
    ReadAttribute(string, data);
  }
}

void XMLObject::ReadAttribute(const std::string& string, XMLLoadData& data)
{
  // Read property name
  std::string property = ReadXMLName(string, data);
  if (property.length() == 0)
    throw XMLException((std::string("Invalid character proceeding name \"") + string[data.pos] + "\".").c_str(), data);
  if (attributes.count(property) > 0)
    throw XMLException((std::string("Duplicate property in XML tag \"") + property + "\".").c_str(), data);
  data.pos += property.length();
  ReadWhiteSpace(string, data);

  // Read =
  if (string[data.pos] != '=')
    throw XMLException((std::string("Invalid character proceeding property name in XML Tag \"") + string[data.pos] + "\".").c_str(), data);
  (data.pos)++;
  ReadWhiteSpace(string, data);

  // Read value
  if (string[data.pos] != '\"')
    throw XMLException("XML property value is not inside enclosing quotes.", data);
  (data.pos)++;
  int valueStart = data.pos;
  while (string[data.pos] != '\"') (data.pos)++;
  std::string value = string.substr(valueStart, (data.pos) - valueStart);
  ReplacePredefinedEntities(value, data);
  (data.pos)++;
  attributes.emplace(property, value);
  ReadWhiteSpace(string, data);
}

void XMLObject::ReadBodyTail(const std::string& string, XMLLoadData& data)
{
  ReadWhiteSpace(string, data);
  if (string[data.pos] != '<')
  {
    ReadText(string, data);
    ReadWhiteSpace(string, data);
    std::string closeTag = GetClosingTag(string, data);
    if (closeTag.length() == 0)
      throw XMLException("Tag after XML Test was not a closing tag. XMLObject doesn't support text and other XMLObjects at the same time.", data);
    return;
  }
  // Check if we can read the closing tag.
  std::string closeTag = GetClosingTag(string, data);
  while (closeTag.length() == 0)
  {
    AddXMLObject(XMLObject(string, data));
    ReadWhiteSpace(string, data);
    closeTag = GetClosingTag(string, data);
  }
}

void XMLObject::ReadText(const std::string& string, XMLLoadData& data)
{
  int startPos = data.pos;
  while (string[data.pos] != '<') (data.pos)++;
  text = string.substr(startPos, (data.pos) - startPos);
  ReplacePredefinedEntities(text, data);
}

void XMLObject::ReadWhiteSpace(const std::string& string, XMLLoadData& data)
{
  while (Utils::IsWhiteSpace(string[data.pos])) {
    if (string[data.pos] == '\n')
      (data.line)++;
    (data.pos)++;
  }
}


std::string XMLObject::GetClosingTag(const std::string& string, XMLLoadData& data)
{
  int startPos = data.pos;
  int startLine = data.line;
  if (string[(data.pos)++] != '<')
  {
    data.pos = startPos;
    data.line = startLine;
    return "";
  }
  ReadWhiteSpace(string, data);
  if (string[(data.pos)++] != '/')
  {
    data.pos = startPos;
    data.line = startLine;
    return "";
  }
  ReadWhiteSpace(string, data);
  std::string tag = Utils::GetWord(string, data.pos);
  if (tag != name)
    throw XMLException((std::string("Closing tag doesn't match opening tag. (\"") + name + "\" != \"" + tag+ "\")").c_str(), data);
  data.pos += tag.length();
  ReadWhiteSpace(string, data);
  if (string[data.pos] != '>')
    throw XMLException((std::string("Invalid character in closing tag \"") + string[data.pos] + "\".").c_str(), data);
  (data.pos)++;
  return string.substr(startPos, (data.pos) - startPos);
}

void XMLObject::ReplacePredefinedEntities(std::string& string, XMLLoadData& data)
{
  std::vector<std::pair<std::string, std::string>> entities
  {
    {"&quot;","\""},
      {"&apos;", "\'"},
      {"&lt;", "<"},
      {"&gt;",">"},
      {"&amp;", "&"}
  };
  size_t pos = string.find('&');
  while(pos != std::string::npos)
  {
    bool found = false;
    for(auto entity : entities)
    {
      if(strncmp(&string[pos], entity.first.c_str(), entity.first.length()) == 0)
      {
        string.replace(pos, entity.first.length(), entity.second);
        found = true;
      }
    }
    if(!found)
      LOG_ERROR("(" + data.file + ":" + std::to_string(data.line) + "): ""Ampersand found in xml but isn't a predefined entity.");
    pos = string.find('&', pos+1);
  }
}

std::string XMLObject::ReadXMLName(const std::string& string, XMLLoadData& data)
{
  if(!(Utils::IsLetter(string[data.pos]) ||
        string[data.pos] == '_' ||
        string[data.pos] == ':'))
    throw XMLException(std::string("Name doesn't start with a letter."), data);

  int endPos = data.pos + 1;
  while (endPos < string.length() && (
        Utils::IsLetter(string[endPos]) ||
        string[endPos] == '_' ||
        string[endPos] == '-' ||
        string[endPos] == ':' ||
        string[endPos] == '.'))
    endPos++;
  return string.substr(data.pos, endPos - data.pos);
}

std::ostream& XMLObject::WriteToStream(std::ostream& stream, int indent) const
{
  for(int i = 0;i<indent;i++)
  {
    stream << "  ";
  }
  stream << "<" << name;
  for(auto it = attributes.begin();it!=attributes.end();++it)
  {
    stream << " " << it->first << "=\"" << it->second << "\"";
  }
  stream << ">";
  if(text != "")
  {
    stream << text;
  }
  else
  {
    bool hasChild = false;
    for(auto it = objects.begin(); it != objects.end();++it)
    {
      for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
      {
        stream << "\n";
        it2->WriteToStream(stream, indent+1);
        hasChild = true;
      }
    }
    if(hasChild)
    {
      stream << "\n";
      for(int i = 0;i<indent;i++)
      {
        stream << "  ";
      }
    }
  }
  stream << "</" << name << ">";

  return stream;
}

