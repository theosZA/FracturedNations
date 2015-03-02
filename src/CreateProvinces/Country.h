#pragma once

#include <string>

class Country
{
  public:
    Country(const std::string& tag) : m_tag(tag) {}

    void ReadFromFile(const std::string& fileName);

    const std::string& GetTag() const       { return m_tag; }
    const std::string& GetName() const      { return m_name; }
    const std::string& GetCulture() const   { return m_culture; }
    const std::string& GetReligion() const  { return m_religion; }

  private:
    std::string m_tag;
    std::string m_name;
    std::string m_culture;
    std::string m_religion;
};