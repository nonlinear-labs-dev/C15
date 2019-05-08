#pragma once

#include <xml/Attribute.h>

class VersionAttribute : public Attribute
{
 public:
  static VersionAttribute &get();

  static int getCurrentFileVersion();

 private:
  VersionAttribute(int v);
  virtual ~VersionAttribute();
};
