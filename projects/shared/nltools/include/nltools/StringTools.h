#pragma once

#include <sstream>
#include <functional>
#include <vector>

namespace nltools
{
  namespace string
  {
    template <typename... Args> std::string concat(Args... args)
    {
      std::stringstream s;
      std::initializer_list<bool> { (s << args, false)... };
      return s.str();
    }

    template <typename... Args> std::string concatWithDelimiter(char delim, Args... args)
    {
      std::stringstream s;
      auto printWithDelim = [&](auto a) { s << a << delim; };
      std::initializer_list<bool> { (printWithDelim(args), false)... };
      return s.str();
    }

    std::string lowercase(const std::string &in);
  }

  void parseURI(
      const std::string &uri,
      std::function<void(const std::string &scheme, const std::string &host, const std::string &path, uint port)> cb);

  bool startsWith(const std::string &string, const std::string &test);

  std::string getFileContent(const std::string &path);
  std::vector<unsigned char> readBinaryFile(const std::string &path);
}
