#pragma once

#include "playground.h"
#include <map>
#include <glib.h>
#include <glibmm/markup.h>

class Attributes
{
 public:
  Attributes();

  virtual ~Attributes();

  void set(size_t keyHash, const Glib::ustring &value);
  Glib::ustring get(size_t keyHash, Glib::ustring defValue = "") const;

  void set(const Glib::ustring &key, const Glib::ustring &value);
  Glib::ustring get(const Glib::ustring &key, Glib::ustring defValue = "") const;

 private:
  map<size_t, Glib::ustring> m_attributes;

  std::hash<Glib::ustring> m_hash;
};
