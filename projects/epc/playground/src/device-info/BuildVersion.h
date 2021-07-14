#pragma once
#include "DeviceInformationItem.h"

class BuildVersion : public DeviceInformationItem
{
 public:
  explicit BuildVersion(DeviceInformation *parent);

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring get() const override;

 private:
  Glib::ustring m_c15VersionString;
};