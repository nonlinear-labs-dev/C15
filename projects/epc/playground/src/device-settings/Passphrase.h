#pragma once

#include "Setting.h"
#include <nltools/messaging/Message.h>
#include "EpcWifi.h"

class Settings;

class Passphrase : public Setting
{
 public:
  explicit Passphrase(Settings& parent, const std::shared_ptr<EpcWifi>& localWifi);
  ~Passphrase() override;
  Glib::ustring getDisplayString() const override;

  void dice();
  void resetToDefault();
  void load(const Glib::ustring& text, Initiator initiator) override;
  Glib::ustring save() const override;

 private:
  void updatePassword(const Glib::ustring& password);
  Glib::ustring m_password;
  const std::shared_ptr<EpcWifi> m_localWifi;
};
