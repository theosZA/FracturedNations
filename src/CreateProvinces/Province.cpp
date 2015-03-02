#include "Province.h"

#include <cctype>
#include <fstream>
#include <sstream>
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

// Find the first trade goods value (other than unknown).
std::string FindTradeGoods(const std::string& sourceFileName)
{
  std::ifstream source(sourceFileName);
  std::string line;
  while (std::getline(source, line))
  {
    if (StringUtility::StartsWith(StringUtility::Trim(line), "trade_goods"))
    {
      auto tradeGoods = StringUtility::GetValueFromKeyValueLine(line);
      if (tradeGoods != "unknown")
        return tradeGoods;
    }
  }
  return "";
}

struct ProvinceWriteState
{
  int commentLines;
  int successiveBlankLines;
  int dateLines;
  bool tagsWritten;
};

struct ProvinceWriteData
{
  bool hasOwner;
  std::string tagsLines;
  std::string culture;
  std::string religion;
  std::string sourceFileName;
};

void WriteProvinceLine(const std::string& line, std::ostream& out, ProvinceWriteState& state, const ProvinceWriteData& data)
{
  // Stop at the first date line.
  if (IsDate(line))
  {
    ++state.dateLines;
    return;
  }

  if (line.empty())
  { // Only write the first blank line in a row.
    if (state.successiveBlankLines == 0)
      out << '\n';
    ++state.successiveBlankLines;
    return;
  }
  state.successiveBlankLines = 0;

  // Write comments.
  if (line[0] == '#')
  {
    out << line << '\n';
    ++state.commentLines;
    return;
  }

  if (!state.tagsWritten)
  { // Write tags.
    out << data.tagsLines;
    state.tagsWritten = true;
  }

  if (data.hasOwner)
  { // Ignore native elements.
    if (StringUtility::StartsWith(line, "native_"))
      return;
    // Replace trade goods if unknown.
    if (StringUtility::StartsWith(line, "trade_goods") && StringUtility::GetValueFromKeyValueLine(line) == "unknown")
    {
      out << "trade_goods = " << FindTradeGoods(data.sourceFileName) << '\n';
      return;
    }
    // Culture is replaced by owner culture.
    if (StringUtility::StartsWith(line, "culture"))
    {
      out << "culture = " << data.culture << '\n';
      return;
    }
    // Religion is replaced by owner religion.
    if (StringUtility::StartsWith(line, "religion"))
    {
      out << "religion = " << data.religion << '\n';
      return;
    }
  }

  // Ignore cores, controller, owner and local autonomy.
  if (StringUtility::StartsWith(line, "add_core") ||
      StringUtility::StartsWith(line, "owner") ||
      StringUtility::StartsWith(line, "controller") ||
      StringUtility::StartsWith(line, "add_local_autonomy"))
    return;
    
  out << line << '\n';
}

void WriteProvinceDateEntry(std::string& currentLine, std::istream& in, std::ostream& out)
{
  out << currentLine << '\n';
  // Write everything (except blank lines) until the next dated entry.
  while (std::getline(in, currentLine))
  {
    if (!currentLine.empty())
    {
      if (IsDate(currentLine))
        return;
      out << currentLine << '\n';
    }
  }
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

  std::ifstream inputFile(sourceFileName);
  std::ofstream outputFile(destFileName);
  std::string line;

  ProvinceWriteData data;
  data.sourceFileName = sourceFileName;
  data.hasOwner = (m_ownerTag != "(none)");
  if (data.hasOwner)
  {
    const auto& country = countries.GetCountry(m_ownerTag);
    data.culture = country.GetCulture();
    data.religion = country.GetReligion();
  }
  std::ostringstream tagsLines;
  if (data.hasOwner)
    tagsLines << "owner = " << m_ownerTag << " # " << countries.GetCountry(m_ownerTag).GetName() << '\n'
              << "controller = " << m_ownerTag << " # " << countries.GetCountry(m_ownerTag).GetName() << '\n';
  for (auto& coreTag : m_coreTags)
  if (coreTag != "(none)")
    tagsLines << "add_core = " << coreTag << " # " << countries.GetCountry(coreTag).GetName() << '\n';
  data.tagsLines = tagsLines.str();

  ProvinceWriteState state;
  state.commentLines = 0;
  state.successiveBlankLines = 0;
  state.dateLines = 0;
  state.tagsWritten = false;

  while (state.dateLines == 0 && std::getline(inputFile, line))
  {
    WriteProvinceLine(line, outputFile, state, data);
  }

  // For the dated entries, we only want the permanent modifiers set at 1000.1.1
  do
  {
    while (StringUtility::StartsWith(line, "1000.1.1"))
    {
      WriteProvinceDateEntry(line, inputFile, outputFile);
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
