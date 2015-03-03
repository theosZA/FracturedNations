#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

#include "CountrySet.h"

class Province
{
  public:
    Province(int id);

    int GetID() const;

    void SetOwner(std::string tag);
    void AddCore(std::string tag);
    void SetCulture(std::string culture);
    void SetReligion(std::string religion);

    void CopyProvinceFile(const std::string& sourceFileName, const std::string& destFileName, const CountrySet& countries) const;

    friend std::ostream& operator<<(std::ostream& out, const Province& province);

  private:
    int m_id;
    std::string m_ownerTag;
    std::unordered_set<std::string> m_coreTags;
    std::string m_culture;  // empty => owner culture
    std::string m_religion; // empty => owner religion
};