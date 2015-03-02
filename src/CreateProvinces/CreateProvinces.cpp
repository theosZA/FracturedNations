#include <fstream>
#include <iostream>
#include <stdexcept>

#include "CountrySet.h"
#include "ProvinceSet.h"

ProvinceSet BuildProvinceSet(const char* provincesFileName)
{
  ProvinceSet provinces;
  std::ifstream provincesInput(provincesFileName);
  while (provincesInput)
  {
    std::string tagInfoLine;
    std::getline(provincesInput, tagInfoLine);
    if (!tagInfoLine.empty())
    {
      auto commentPos = tagInfoLine.find('#'); // ignore everything from the # onwards
      provinces.ParseTagInfo(tagInfoLine.substr(0, commentPos));
    }
  }
  return provinces;
}

int main(int argc, char* argv[])
{
  const char* const countriesHistoryPath = "\\history\\countries";
  const char* const provincesHistoryPath = "\\history\\provinces";

  try
  {
    if (argc < 4)
    {
      std::cout << "Insufficient arguments\nFORMAT: CreateProvinces [provinces file] [EU4 path] [mod path]" << std::endl;
      return 1;
    }
    const char* provincesFileName = argv[1];
    std::string eu4Path = argv[2];
    std::string modPath = argv[3];

    CountrySet countries;
    countries.ReadCountries(modPath + countriesHistoryPath, true);
    countries.ReadCountries(eu4Path + countriesHistoryPath, false);

    auto provinces = BuildProvinceSet(provincesFileName);
    provinces.CopyProvinceFiles(eu4Path + provincesHistoryPath, modPath + provincesHistoryPath, countries);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}