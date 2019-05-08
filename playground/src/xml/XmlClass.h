#pragma once

#include "Attribute.h"

class XmlClass : public Attribute
{
 public:
  XmlClass(const Glib::ustring &classes);
  virtual ~XmlClass();
};
