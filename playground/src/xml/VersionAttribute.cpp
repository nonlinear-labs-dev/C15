#include <xml/VersionAttribute.h>

VersionAttribute::VersionAttribute(int v)
    : Attribute("version", v)
{
}

VersionAttribute::~VersionAttribute()
{
}

VersionAttribute &VersionAttribute::get()
{
  static VersionAttribute currentFileVersion(getCurrentFileVersion());
  return currentFileVersion;
}

int VersionAttribute::getCurrentFileVersion()
{
  return 7;
}
