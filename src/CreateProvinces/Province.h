#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

#include "StringUtility.h"

class Province
{
  public:
    Province(int id) : m_id(id) {}

    int GetID() const
    {
      return m_id;
    }

    void SetOwner(std::string tag)
    {
      AddCore(tag);
      m_ownerTag = std::move(tag);
    }

    void AddCore(std::string tag)
    {
      m_coreTags.insert(std::move(tag));
    }

    void CopyProvinceFile(const std::string& sourceFileName, const std::string& destFileName) const
    {
      std::ifstream inputFile(sourceFileName);
      std::ofstream outputFile(destFileName);
      // Copy everything until the first dated entries.
      std::string line;
      bool datedEntry = false;
      while (!datedEntry && std::getline(inputFile, line))
      {
        datedEntry = (!line.empty() && line[0] == '1'); // indicates the line begins with a date - e.g. 1436.4.28
        if (!datedEntry)
        {
          // Ignore cores, controller, owner.
          if (!StringUtility::StartsWith(line, "add_core") &&
              !StringUtility::StartsWith(line, "owner") && 
              !StringUtility::StartsWith(line, "controller"))
            outputFile << line << '\n';
        }
      }
      // Write the new cores, controller, owner.
      if (m_ownerTag != "(none)")
        outputFile << "owner = " << m_ownerTag << '\n'
                   << "controller = " << m_ownerTag << '\n';
      for (auto& coreTag : m_coreTags)
        if (coreTag != "(none)")
          outputFile << "add_core = " << coreTag << '\n';
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
            datedEntry = (!line.empty() && line[0] == '1'); // indicates the line begins with a date - e.g. 1436.4.28
            if (!datedEntry)
              outputFile << line << '\n';
          }
        }
      }
      while (std::getline(inputFile, line));
    }

    bool operator<(const Province& comp) const
    {
      return m_id < comp.m_id;
    }

    friend std::ostream& operator<<(std::ostream& out, const Province& province)
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

  private:
    int m_id;
    std::string m_ownerTag;
    std::unordered_set<std::string> m_coreTags;
};