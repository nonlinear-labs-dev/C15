#pragma once

#include "device-settings/Settings.h"
#include "Serializer.h"

class SettingsSerializer : public Serializer
{
 public:
  SettingsSerializer(Settings &settings);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  Settings &m_settings;
};
