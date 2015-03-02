#include "Country.h"

#include <fstream>

#include "StringUtility.h"

std::string GetValueFromKeyValueLine(const std::string& keyValueLine)
{
  auto equalPos = keyValueLine.find('=');
  if (equalPos == std::string::npos || equalPos + 1 == keyValueLine.size())
    return "";

  return StringUtility::Trim(keyValueLine.substr(equalPos + 1));
}

std::string GetCountryNameFromFileName(const std::string& fileName)
{
  auto dashPos = fileName.find_last_of('-');
  if (dashPos == std::string::npos)
    return "";
  auto dotPos = fileName.find_last_of('.');
  if (dotPos == std::string::npos || dotPos < dashPos)
    return "";

  return StringUtility::Trim(fileName.substr(dashPos + 1, dotPos - (dashPos + 1)));
}

void Country::ReadFromFile(const std::string& fileName)
{
  m_name = GetCountryNameFromFileName(fileName);

  std::ifstream inputFile(fileName);
  std::string line;
  while (std::getline(inputFile, line))
  {
    if (StringUtility::StartsWith(line, "religion"))
      m_religion = GetValueFromKeyValueLine(line);
    else if (StringUtility::StartsWith(line, "primary_culture"))
      m_culture = GetValueFromKeyValueLine(line);
  }
}
