#pragma once

#include "playground.h"
#include <sstream>

namespace attribute_detail
{
  template <typename tValue> inline ustring toString(const tValue &v)
  {
    std::stringstream str;
    str << v;
    return str.str();
  }

  template <> inline ustring toString(const ustring &v)
  {
    return v;
  }
}

class Attribute
{
 public:
  template <typename tValue> inline Attribute(const Glib::ustring &name, const tValue &v)
  {
    m_name = name;
    m_value = attribute_detail::toString(v);
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