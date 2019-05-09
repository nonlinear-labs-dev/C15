#pragma once

#include <glibmm/ustring.h>
#include <vector>
#include <libundo/undo/StringTools.h>

namespace StringTools
{
  std::vector<Glib::ustring> splitStringOnAnyDelimiter(const Glib::ustring& s, char delimiter);
  std::vector<std::string> splitStringOnStringDelimiter(std::string s, std::string delimiter);
  Glib::ustring replaceAll(const Glib::ustring& in, const Glib::ustring& pattern, const Glib::ustring& replace);
  Glib::ustring removeSpaces(const Glib::ustring& in);

  template <typename... tParts> Glib::ustring formatString(const Glib::ustring& format, const tParts&... parts)
  {
    return UNDO::StringTools::formatString(format, parts...);
  }

  template <typename... tParts> Glib::ustring buildString(const tParts&... parts)
  {
    return UNDO::StringTools::buildString(parts...);
  }
};
