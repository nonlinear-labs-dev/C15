#pragma once

#include "DeviceInformationItem.h"

class SoftwareVersion : public DeviceInformationItem
{
 public:
  explicit SoftwareVersion(DeviceInformation *parent);

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring get() const override;

 private:
  Glib::ustring m_versionString;
};
