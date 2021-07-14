#pragma once

#include <http/UpdateDocumentContributor.h>
#include <device-info/DeviceInformation.h>
#include <tools/Signal.h>

class DeviceInformationItem : public UpdateDocumentContributor
{
 public:
  explicit DeviceInformationItem(DeviceInformation *parent);

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
  sigc::connection onChange(sigc::slot<void, const DeviceInformationItem *> slot);

  virtual Glib::ustring get() const = 0;
  virtual Glib::ustring getDisplayString() const;

 private:
  Signal<void, const DeviceInformationItem *> m_signal;
};
