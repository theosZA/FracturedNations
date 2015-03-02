#include "Province.h"

#include <cctype>
#include <fstream>
#include <stdexcept>

#include "StringUtility.h"

bool IsDate(const std::string& text)
{
  if (text.empty())
    return false;

  // Match pattern "dddd.*" or "# dddd.*"
  size_t startPos = 0;
  if (text[0] == '#')
  {
    ++startPos;
    while (startPos < text.size() && text[startPos] == ' ')
      ++startPos;
  }
  if (text.size() < startPos + 5)
    return false; // not long enough to even have a year portion
  return std::isdigit(static_cast<unsigned char>(text[startPos])) &&
         std::isdigit(static_cast<unsigned char>(text[startPos + 1])) &&
         std::isdigit(static_cast<unsigned char>(text[startPos + 2])) &&
         std::isdigit(static_cast<unsigned char>(text[startPos + 3])) &&
         text[startPos + 4] == '.';
}

Province::Province(int id) 
: m_id(id) 
{}

int Province::GetID() const
{
  return m_id;
}

void Province::SetOwner(std::string tag)
{
  AddCore(tag);
  m_ownerTag = std::move(tag);
}

void Province::AddCore(std::string tag)
{
  m_coreTags.insert(std::move(tag));
}

void Province::CopyProvinceFile(const std::string& sourceFileName, const std::string& destFileName, const CountrySet& countries) const
{
  if (m_ownerTag.empty())
    throw std::runtime_error("Province " + std::to_string(m_id) + " does not have an owner");
  bool hasOwner = (m_ownerTag != "(none)");

  std::ifstream inputFile(sourceFileName);
  std::ofstream outputFile(destFileName);
  // Copy everything until the first dated entries.
  std::string line;
  bool commented = false;
  bool previousLineWasBlank = false;
  bool datedEntry = false;
  while (!datedEntry && std::getline(inputFile, line))
  {
    datedEntry = IsDate(line);
    if (!datedEntry)
    {
      // Ignore successive blank lines in sequence.
      if (line.empty())
      {
        if (!previousLineWasBlank)
          outputFile << line << '\n';
        previousLineWasBlank = true;
      }
      else
      {
        previousLineWasBlank = false;
        // Culture/religion are replaced by owner culture/religion.
        if (hasOwner && StringUtility::StartsWith(line, "culture"))
          outputFile << "culture = " << countries.GetCountry(m_ownerTag).GetCulture() << '\n';
        else if (hasOwner && StringUtility::StartsWith(line, "religion"))
          outputFile << "religion = " << countries.GetCountry(m_ownerTag).GetReligion() << '\n';
        // Ignore all but the first comment line.
        else if (line[0] == '#')
        {
          if (!commented)
            outputFile << line << '\n';
          commented = true;
        }
        // Ignore cores, controller, owner (and local autonomy).
        else if (!StringUtility::StartsWith(line, "add_core") &&
                 !StringUtility::StartsWith(line, "owner") &&
                 !StringUtility::StartsWith(line, "controller") &&
                 !StringUtility::StartsWith(line, "add_local_autonomy"))
          outputFile << line << '\n';
      }
    }
  }
  // Write the new cores, controller, owner.
  if (hasOwner)
    outputFile << "owner = " << m_ownerTag << " # " << countries.GetCountry(m_ownerTag).GetName() << '\n'
               << "controller = " << m_ownerTag << " # " << countries.GetCountry(m_ownerTag).GetName() << '\n';
  for (auto& coreTag : m_coreTags)
    if (coreTag != "(none)")
      outputFile << "add_core = " << coreTag << " # " << countries.GetCountry(coreTag).GetName() << '\n';
  // For the dated entries, we only want the permanent modifiers set at 1000.1.1
  do
  {
    while (StringUtility::StartsWith(line, "1000.1.1"))
    {
      outputFile << '\n' << line << '\n';
      // Copy everything until the next dated entry.
      bool datedEntry = false;
      while (std::getline(inputFile, line) && !datedEntry)
      {
        if (!line.empty())
        {
          datedEntry = IsDate(line);
          if (!datedEntry)
            outputFile << line << '\n';
        }
      }
    }
  } while (std::getline(inputFile, line));
}

std::ostream& operator<<(std::ostream& out, const Province& province)
{
  out << '[' << province.m_id << "] Owned by " << province.m_ownerTag << " (cores: ";
  bool first = true;
  for (auto& core : province.m_coreTags)
  {
    if (!first)
      out << ", ";
    out << core;
    first = false;
  }
  out << ')';
  return out;
}
