#pragma once

#include "DeviceInformationItem.h"

class FreeDiscSpaceInformation : public DeviceInformationItem
{
 public:
  explicit FreeDiscSpaceInformation(DeviceInformation *parent);

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  virtual Glib::ustring get() const override;

 private:
  bool refresh();

  Glib::ustring m_value;
};
