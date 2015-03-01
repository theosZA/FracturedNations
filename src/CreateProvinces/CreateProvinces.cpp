#include <fstream>
#include <iostream>

#include "ProvinceSet.h"

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cout << "Insufficient arguments\nFORMAT: CreateProvinces [province file] [source province path] [destination province path]" << std::endl;
    return 1;
  }
  std::ifstream provincesInput(argv[1]);
  ProvinceSet provinces;
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

  provinces.CopyProvinceFiles(argv[2], argv[3]);

	//std::cout << provinces << std::endl;
}