#pragma once

#include <map>
#include <string>

#include "Country.h"

class CountrySet
{
  public:
    void ReadCountries(const std::string& path, bool overwriteDuplicates);

    const Country& GetCountry(const std::string& tag) const;

  private:
    void ReadCountry(const std::string& path, const std::string& fileName, bool overwriteDuplicates);

    std::map<std::string, Country> m_countries;
};