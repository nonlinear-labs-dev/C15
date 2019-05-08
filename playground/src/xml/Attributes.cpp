#include "Attributes.h"

Attributes::Attributes()
{
}

Attributes::~Attributes()
{
}

void Attributes::set(size_t keyHash, const Glib::ustring &value)
{
  m_attributes[keyHash] = value;
}

Glib::ustring Attributes::get(size_t keyHash, Glib::ustring defValue) const
{
  auto it = m_attributes.find(keyHash);

  if(it != m_attributes.end())
    return it->second;

  return defValue;
}

void Attributes::set(const Glib::ustring &key, const Glib::ustring &value)
{
  set(m_hash(key), value);
}

Glib::ustring Attributes::get(const Glib::ustring &key, Glib::ustring defValue) const
{
  return get(m_hash(key), defValue);
}
