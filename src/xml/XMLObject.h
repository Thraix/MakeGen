#pragma once

#include <string>
#include <map>
#include <set>
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
    std::map<std::string, std::vector<XMLObject>> objects;

  public:
    XMLObject() {}
    XMLObject(const std::string& string);
    XMLObject(const std::string& string, int pos, int line, const std::string& file);
    XMLObject(const std::string& string, XMLLoadData& data);
    XMLObject(const std::string& name, const std::map<std::string, std::string>& properties, const std::string& text);
    XMLObject(const std::string& name, const std::map<std::string, std::string>& properties, const std::map<std::string, std::vector<XMLObject>>& objects);

    bool HasAttribute(const std::string& property) const;
    const std::string& GetAttribute(const std::string& property) const;
    const std::string& GetAttribute(const std::string& property, const std::string& defaultValue) const;

    unsigned int GetObjectCount() const;
    std::vector<XMLObject>* GetObjectPtr(const std::string& name);
    const std::map<std::string, std::vector<XMLObject>>& GetObjects() const;
    const std::string& GetName() const;
    const std::string& GetText() const;
    XMLObject GetStrippedXMLObject() const;

    void SetName(const std::string& name);
    void SetText(const std::string& text);
    void AddAttribute(const std::string& property, const std::string& value);
    void AddXMLObject(const XMLObject& object);
    bool RemoveXMLObject(const XMLObject& object);

    friend bool operator<(const XMLObject& obj1, const XMLObject& obj2)
    {
      return obj1.name < obj2.name;
    }

    std::ostream& WriteToStream(std::ostream& stream, int indent = 0) const;
    friend std::ostream& operator<<(std::ostream& stream, const XMLObject& object)
    {
      return object.WriteToStream(stream);
    }

    friend bool operator==(const XMLObject& object1, const XMLObject& object2)
    {
      if(object1.attributes.size() != object2.attributes.size())
        return false;
      if(object1.objects.size() != object2.objects.size())
        return false;
      if(object1.name != object2.name)
        return false;
      if(object1.GetText() != object2.GetText())
        return false;

      {
        auto it1 = object1.attributes.begin();
        auto it2 = object2.attributes.begin();
        while(it1 != object1.attributes.end())
        {
          if(it1->first != it2->first || it1->second != it1->second)
            return false;
          ++it1;
          ++it2;
        }
      }

      {
        auto it1 = object1.objects.begin();
        auto it2 = object2.objects.begin();
        while(it1 != object1.objects.end())
        {
          if(it1->second != it2->second)
            return false;
          ++it1;
          ++it2;
        }
      }
      return true;
    }

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
