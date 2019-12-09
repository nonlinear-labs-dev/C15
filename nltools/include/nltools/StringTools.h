#pragma once

#include <sstream>
#include <functional>

namespace nltools
{
  namespace string
  {
    template <typename... Args> std::string concat(Args... args)
    {
      std::stringstream s;
      std::initializer_list<bool>{ (s << args, false)... };
      return s.str();
    }
  }

  namespace Truncate
  {
    std::string trimEnd(const std::string &in);
    std::string numberWithoutZeros(const std::string &in);
  }

  std::vector<std::string> splitStringOnAnyDelimiter(const std::string &s, char delimiter);

  void parseURI(
      const std::string &uri,
      std::function<void(const std::string &scheme, const std::string &host, const std::string &path, uint port)> cb);

  bool startsWith(const std::string &string, const std::string &test);
}
