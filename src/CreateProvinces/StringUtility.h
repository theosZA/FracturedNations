#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace StringUtility {

inline std::vector<std::string> Split(const std::string& source, char delim)
{
  std::vector<std::string> items;
  std::string item;
  std::istringstream stream(source);
  while (std::getline(stream, item, delim))
  {
    items.push_back(item);
  }
  return items;
}

inline std::string Trim(const std::string& source, const std::string& whitespace = " \t")
{
  auto sourceBegin = source.find_first_not_of(whitespace);
  if (sourceBegin == std::string::npos)
    return ""; // no content
  auto sourceEnd = source.find_last_not_of(whitespace);

  return source.substr(sourceBegin, sourceEnd - sourceBegin + 1);
}

inline bool StartsWith(const std::string& s, const std::string& substring)
{
  return s.compare(0, substring.size(), substring) == 0;
}

inline std::string GetValueFromKeyValueLine(const std::string& keyValueLine)
{
  auto equalPos = keyValueLine.find('=');
  if (equalPos == std::string::npos || equalPos + 1 == keyValueLine.size())
    return "";

  return StringUtility::Trim(keyValueLine.substr(equalPos + 1));
}

}