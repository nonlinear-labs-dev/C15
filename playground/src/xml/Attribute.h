#pragma once

#include "playground.h"
#include <sstream>

class Attribute
{
 public:
  template <typename tValue> inline Attribute(const Glib::ustring &name, const tValue &v)
  {
    m_name = name;
    stringstream str;
    str << v;
    m_value = str.str();
  }

  virtual ~Attribute();

  Glib::ustring toXmlString() const
  {
    return Markup::escape_text(m_name) + "=\"" + Markup::escape_text(m_value) + "\"";
  }

  const Glib::ustring &getName() const
  {
    return m_name;
  }

  const Glib::ustring &getValue() const
  {
    return m_value;
  }

 private:
  Glib::ustring m_name;
  Glib::ustring m_value;
};

template <> inline Attribute::Attribute<ustring>(const Glib::ustring &name, const ustring &v)
{
  m_name = name;
  m_value = v;
}
