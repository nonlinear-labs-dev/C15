#pragma once

#include "Setting.h"
#include <nltools/messaging/Message.h>

class Settings;

class Passphrase : public Setting
{
 public:
  Passphrase(Settings& parent);
  virtual ~Passphrase();

  Glib::ustring getDisplayString() const override;

  void dice();
  void resetToDefault();
  void load(const Glib::ustring& text, Initiator initiator) override;
  Glib::ustring save() const override;

 private:
  void updatePassword(const Glib::ustring& password);
  Glib::ustring m_password;
};