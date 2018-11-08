
#include <vector>
#include <algorithm>
#include "StringTools.h"

namespace StringTools
{

  std::vector<Glib::ustring> splitStringOnAnyDelimiter(const Glib::ustring &s, char delimiter)
  {
    std::vector<Glib::ustring> strings;
    std::string token;
    std::istringstream tokenStream(s);
    while(std::getline(tokenStream, token, delimiter))
    {
      strings.push_back(token);
    }
    return strings;
  }

  Glib::ustring removeSpaces(const Glib::ustring &in)
  {
    std::string ret = in;
    ret.erase(std::remove_if(ret.begin(), ret.end(), [&](char c){ return std::isspace(c);}), ret.end());
    return ret;
  }

  Glib::ustring replaceAll(const Glib::ustring &in, const Glib::ustring &pattern, const Glib::ustring &replace)
  {
    Glib::ustring ret(in);
    size_t pos = 0;
    while((pos = ret.find(pattern, pos)) != Glib::ustring::npos)
    {
      ret.replace(pos, pattern.size(), replace);
      pos += replace.size();
    }
    return ret;
  }
}
