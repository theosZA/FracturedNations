#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "StringUtility.h"
#include "Province.h"

class ProvinceSet
{
  public:
    void ParseTagInfo(const std::string& tagInfo);

    void CopyProvinceFiles(const char* sourcePath, const char* destPath) const;

    friend std::ostream& operator<<(std::ostream& out, const ProvinceSet& provinces);

  private:
    std::map<int, Province> m_provinces;
};