#pragma once

#include <nltools/messaging/Message.h>
#include "Setting.h"

class Settings;

class SSID : public Setting
{
 public:
  Glib::ustring getDisplayString() const override;

  explicit SSID(Settings& parent);
  ~SSID() override;

  bool persistent() const override;
  void load(const Glib::ustring& text) override;
  Glib::ustring save() const override;

 private:
  void setSSIDAndNotifyBBB(const Glib::ustring& str);
  Glib::ustring m_ssid;
};
