#pragma once

#include <string>
#include <map>
#include <vector>

class XMLObject
{
  public:
    friend class XMLexception;
    struct XMLLoadData
    {
      int pos;
      int line;
      const std::string& file;
    };
  private:
    std::string name;
    std::string text;

    std::map<std::string, std::string> attributes;
    std::vector<XMLObject> objects;

  public:
    XMLObject() {}
    XMLObject(const std::string& string);
    XMLObject(const std::string& string, int pos, int line, const std::string& file);
    XMLObject(const std::string& string, XMLLoadData& data);
    XMLObject(const std::string& name, const std::map<std::string, std::string>& properties, const std::string& text);
    XMLObject(const std::string& name, const std::map<std::string, std::string>& properties, const std::vector<XMLObject>& objects);

    bool HasAttribute(const std::string& property) const;
    const std::string& GetAttribute(const std::string& property) const;
    const std::string& GetAttribute(const std::string& property, const std::string& defaultValue) const;

    unsigned int GetObjectCount() const;
    const XMLObject& GetObject(unsigned int i) const;
    const std::vector<XMLObject>& GetObjects() const;
    const std::string& GetName() const;
    const std::string& GetText() const;
    XMLObject GetStrippedXMLObject() const;

    void SetName(const std::string& name);
    void SetText(const std::string& text);
    void AddAttribute(const std::string& property, const std::string& value);

  private:
    std::string GetClosingTag(const std::string& string, XMLLoadData& data);
    // Returns true if the head contained closing tag.
    bool ReadHead(const std::string& string, XMLLoadData& data);
    void ReadName(const std::string& string, XMLLoadData& data);
    void ReadAttribute(const std::string& string, XMLLoadData& data);
    void ReadAttributes(const std::string& string, XMLLoadData& data);
    void ReadBodyTail(const std::string& string, XMLLoadData& data);
    void ReadText(const std::string& string, XMLLoadData& data);
    void ReadWhiteSpace(const std::string& string, XMLLoadData& data);
    void ReplacePredefinedEntities(std::string& string, XMLLoadData& data);
    std::string ReadXMLName(const std::string& string, XMLLoadData& data);
};
