#pragma once

#include <glibmm/ustring.h>

namespace StringTools {
    std::vector<Glib::ustring> splitStringOnAnyDelimiter(const Glib::ustring& s, char delimiter);
    Glib::ustring removeSpaces(const Glib::ustring& in);
    Glib::ustring replaceAll(const Glib::ustring& in, const Glib::ustring& pattern, const Glib::ustring& replace);
};
