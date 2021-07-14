#pragma once

#include "DeviceInformationItem.h"

class RTSoftwareVersion : public DeviceInformationItem
{
 public:
  explicit RTSoftwareVersion(DeviceInformation *parent);

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring get() const override;

 private:
  void onVersionChanged(int v);
  sigc::connection m_connection;
};