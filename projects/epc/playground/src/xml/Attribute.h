#pragma once

#include "playground.h"

class ParameterId;
class GroupId;

namespace attribute_detail
{
  template <typename tValue> inline Glib::ustring toString(const tValue &v)
  {
    return std::to_string(v);
  }

  Glib::ustring toString(const ParameterId &v);
  Glib::ustring toString(const GroupId &v);
  Glib::ustring toString(const Glib::ustring &v);
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

  [[nodiscard]] Glib::ustring toXmlString() const;
  [[nodiscard]] const Glib::ustring &getName() const;
  [[nodiscard]] const Glib::ustring &getValue() const;

 private:
  Glib::ustring m_name;
  Glib::ustring m_value;
};
