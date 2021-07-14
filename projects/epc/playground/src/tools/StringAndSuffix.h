#pragma once
#include <utility>
#include <glibmm/ustring.h>

class StringAndSuffix
{
 public:
  static StringAndSuffix empty();
  StringAndSuffix(const Glib::ustring &first, const Glib::ustring &second);
  StringAndSuffix(Glib::ustring first, size_t suffix = 0);
  StringAndSuffix(const char *first, size_t suffix = 0);
  StringAndSuffix(char c, size_t suffix = 0);

  bool operator!=(const StringAndSuffix &other) const;

  [[nodiscard]] std::pair<Glib::ustring, Glib::ustring> getSplits() const;

  Glib::ustring text;
  size_t suffix = 0;
};
