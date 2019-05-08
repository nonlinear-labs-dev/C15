#pragma once

#include "DeviceInformationItem.h"

class OSVersion : public DeviceInformationItem
{
 public:
  OSVersion(DeviceInformation *parent);

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring get() const override;

 private:
  Glib::ustring m_versionString;
};
