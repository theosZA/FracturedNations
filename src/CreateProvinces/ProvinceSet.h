#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "CountrySet.h"
#include "Province.h"

class ProvinceSet
{
  public:
    void ParseTagInfo(const std::string& tagInfo);

    void CopyProvinceFiles(const std::string& sourcePath, const std::string& destPath, const CountrySet& countries) const;

    friend std::ostream& operator<<(std::ostream& out, const ProvinceSet& provinces);

  private:
    std::map<int, Province> m_provinces;
};