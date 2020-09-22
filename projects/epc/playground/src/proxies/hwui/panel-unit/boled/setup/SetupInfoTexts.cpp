#include <nltools/Assert.h>
#include <iostream>
#include "SetupInfoTexts.h"

std::map<SetupInfoEntries, std::string> SetupInfoTexts::m_headers {};
std::map<SetupInfoEntries, std::string> SetupInfoTexts::m_content {};

void SetupInfoTexts::loadInfos()
{
  std::ifstream stream(getResourcesDir() + "/infos.json");
  std::string data;
  std::string line;
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      data += line;
    }
  }

  nlohmann::json in(data);

  for(auto& e: in) {
    std::cout << nlohmann::json(e)[0] << std::endl;
  }

  for(auto e : getAllValues<SetupInfoEntries>())
  {
    if(auto it = in.find(toString(e)); it != in.end())
    {
      auto val = *it;
      m_headers[e] = val["Header"];
      m_content[e] = val["Content"];
    }
    else
    {
      auto msg = toString(e) + " not found in infos.json";
      nltools::fail(msg.data(), __FILE__, __LINE__, __PRETTY_FUNCTION__);
    }
  }
}

std::string SetupInfoTexts::getInfoHeader(SetupInfoEntries e)
{
  return m_headers.at(e);
}

std::string SetupInfoTexts::getInfoContent(SetupInfoEntries e)
{
  return m_content.at(e);
}
