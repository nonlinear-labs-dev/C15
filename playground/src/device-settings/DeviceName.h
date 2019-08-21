#pragma once

#include "Setting.h"

class Settings;

class DeviceName : public Setting
{
 private:
  typedef Setting super;

 public:
  DeviceName(Settings &parent);
  virtual ~DeviceName();

  ustring getDisplayString() const override;

  void sanitize();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void set(Glib::ustring name);
  const Glib::ustring &get() const;

 private:
  Glib::ustring m_name;
};
