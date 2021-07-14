#pragma once

#include "device-settings/Settings.h"
#include "Serializer.h"

class SettingsSerializer : public Serializer
{
 public:
  explicit SettingsSerializer(Settings &settings);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  Settings &m_settings;
};
