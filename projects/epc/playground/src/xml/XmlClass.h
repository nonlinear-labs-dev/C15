#pragma once

#include "Attribute.h"

class XmlClass : public Attribute
{
 public:
  explicit XmlClass(const Glib::ustring &classes);
  ~XmlClass() override;
};
