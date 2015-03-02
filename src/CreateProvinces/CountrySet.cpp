#include "CountrySet.h"

#include <stdexcept>

#include <dirent\dirent.h>

void CountrySet::ReadCountries(const std::string& path, bool overwriteDuplicates)
{
  struct dirent* ent;
  DIR* dir = opendir(path.c_str());
  if (!dir) // path not found
    return;

  while (ent = readdir(dir))
  {
    ReadCountry(path, ent->d_name, overwriteDuplicates);
  }
  closedir(dir);
}

const Country& CountrySet::GetCountry(const std::string& tag) const
{
  auto findIter = m_countries.find(tag);
  if (findIter == m_countries.end())
    throw std::runtime_error(tag + " not found in country set");
  return findIter->second;
}

void CountrySet::ReadCountry(const std::string& path, const std::string& fileName, bool overwriteDuplicates)
{
  if (fileName.size() < 3)
    return; // file name too short for a tag

  std::string tag = fileName.substr(0, 3);
  auto findIter = m_countries.find(tag);
  bool isDuplicate = (findIter != m_countries.end());
  if (isDuplicate && !overwriteDuplicates)
    return; // tag already exists and we aren't allowed to overwrite

  Country country(tag);
  country.ReadFromFile(path + "\\" + fileName);
  
  if (isDuplicate)
    findIter->second = std::move(country);
  else
    m_countries.emplace(tag, std::move(country));
}
