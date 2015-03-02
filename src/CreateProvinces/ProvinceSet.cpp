#include "ProvinceSet.h"

#include <dirent\dirent.h>

#include "StringUtility.h"

void ProvinceSet::ParseTagInfo(const std::string& tagInfo)
{
  // Get tag.
  auto colonPos = tagInfo.find(':');
  if (colonPos == std::string::npos)
    return;
  auto tag = tagInfo.substr(0, colonPos);
  // Get provinces.
  auto provinces = StringUtility::Split(tagInfo.substr(colonPos + 1), ',');
  bool owns = true;
  for (auto& provinceIDStr : provinces)
  {
    provinceIDStr = StringUtility::Trim(provinceIDStr);
    if (provinceIDStr == "(cores)")
      owns = false;
    else
    {
      auto provinceID = std::stoi(provinceIDStr);
      auto provinceIter = m_provinces.find(provinceID);
      if (provinceIter == m_provinces.end())
        provinceIter = m_provinces.insert(std::make_pair(provinceID, Province(provinceID))).first;
      auto& province = provinceIter->second;
      if (owns)
        province.SetOwner(tag);
      else
        province.AddCore(tag);
    }
  }
}

void ProvinceSet::CopyProvinceFiles(const std::string& sourcePath, const std::string& destPath, const CountrySet& countries) const
{
  struct dirent* ent;
  DIR* dir = opendir(sourcePath.c_str());
  if (!dir) // path not found
    return;

  while (ent = readdir(dir))
  {
    std::string fileName = ent->d_name;
    auto nonNumericPos = fileName.find_first_not_of("0123456789");
    if (nonNumericPos != 0)
    {
      int provinceID = std::stoi(fileName.substr(0, nonNumericPos));
      auto findIter = m_provinces.find(provinceID);
      if (findIter != m_provinces.end())
      {
        findIter->second.CopyProvinceFile(sourcePath + "\\" + fileName,
                                          destPath + "\\" + fileName,
                                          countries);
      }
    }
  }
  closedir(dir);
}

std::ostream& operator<<(std::ostream& out, const ProvinceSet& provinces)
{
  for (auto& province : provinces.m_provinces)
    out << province.second << std::endl;
  return out;
}