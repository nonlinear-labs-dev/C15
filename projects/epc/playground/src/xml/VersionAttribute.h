#pragma once

#include <xml/Attribute.h>

class VersionAttribute : public Attribute
{
 public:
  static VersionAttribute &get();

  static int getCurrentFileVersion();

 private:
  explicit VersionAttribute(int v);
  ~VersionAttribute() override;
};
