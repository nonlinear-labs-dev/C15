#pragma once

#include "Setting.h"
#include "tools/Expiration.h"

class Settings;

class HostAPDSetting : public Setting
{
 private:
  typedef Setting super;

 public:
  HostAPDSetting(Settings &parent, const std::string &pattern);
  virtual ~HostAPDSetting();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void set(Glib::ustring name);
  const Glib::ustring &get() const;

 protected:
  void loadFromFile();

 private:
  void saveToFile();

  static void restartAccessPoint();

  Glib::ustring m_setting;
  std::string m_pattern;
};
