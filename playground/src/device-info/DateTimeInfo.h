#pragma once

#include "DeviceInformationItem.h"

class DateTimeInfo : public DeviceInformationItem
{
 public:
  DateTimeInfo(DeviceInformation *parent);

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  Glib::ustring get() const override;
  Glib::ustring getDisplayString() const override;
  bool onTimeChanged();
};
