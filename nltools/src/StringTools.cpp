#include <nltools/StringTools.h>
#include <libsoup/soup.h>
#include <algorithm>

namespace nltools
{
  void parseURI(
      const std::string &uri,
      std::function<void(const std::string &scheme, const std::string &host, const std::string &path, uint port)> cb)
  {
    auto u = soup_uri_new(uri.c_str());
    cb(soup_uri_get_scheme(u), soup_uri_get_host(u), soup_uri_get_path(u), soup_uri_get_port(u));
    soup_uri_free(u);
  }

  bool startsWith(const std::string &string, const std::string &test)
  {
    return g_str_has_prefix(string.c_str(), test.c_str());
  }

  std::vector<std::string> splitStringOnAnyDelimiter(const std::string &s, char delimiter)
  {
    std::vector<std::string> strings;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter))
    {
      strings.emplace_back(token);
    }
    return strings;
  }

  namespace Truncate
  {
    std::string trimEnd(const std::string &in)
    {
      std::string ret{};
      for(auto it = in.rbegin(); it != in.rend(); it++)
      {
        if(*it != '0')
          ret += *it;
      }

      return ret;
    }

    std::string numberWithoutZeros(const std::string &in)
    {
      auto parts = nltools::splitStringOnAnyDelimiter(in, '.');
      if(parts.size() == 2)
      {
        auto secondHalf = trimEnd(parts[1]);
        if(secondHalf.empty())
        {
          return parts[0];
        }
        else
        {
          return parts[0] + "." + secondHalf;
        }
      }

      return in;
    }
  }
}
