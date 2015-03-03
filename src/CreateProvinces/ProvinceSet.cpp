#include "ProvinceSet.h"

#include <stdexcept>

#include <dirent\dirent.h>

#include "StringUtility.h"

void SplitProvinceInfo(const std::string& provinceInfo, int& provinceID, std::string& culture, std::string& religion)
{
  auto items = StringUtility::Split(provinceInfo, '|');
  if (items.empty())
    throw std::runtime_error("No province ID in province info");
  provinceID = std::stoi(items[0]);
  for (size_t i = 1; i < items.size(); ++i)
  {
    auto key = StringUtility::GetKeyFromKeyValueLine(items[i]);
    auto value = StringUtility::GetValueFromKeyValueLine(items[i]);
    if (key == "c")
      culture = value;
    else if (key == "r")
      religion = value;
    else
      throw std::runtime_error("Unknown province info key '" + key + "' for province " + items[0]);
  }
}

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
  for (auto& provinceInfo : provinces)
  {
    provinceInfo = StringUtility::Trim(provinceInfo);
    if (provinceInfo == "(cores)")
      owns = false;
    else
    {
      int provinceID;
      std::string culture;
      std::string religion;
      SplitProvinceInfo(provinceInfo, provinceID, culture, religion);
      auto provinceIter = m_provinces.find(provinceID);
      if (provinceIter == m_provinces.end())
        provinceIter = m_provinces.insert(std::make_pair(provinceID, Province(provinceID))).first;
      auto& province = provinceIter->second;
      if (owns)
        province.SetOwner(tag);
      else
        province.AddCore(tag);
      if (!culture.empty())
        province.SetCulture(culture);
      if (!religion.empty())
        province.SetReligion(religion);
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