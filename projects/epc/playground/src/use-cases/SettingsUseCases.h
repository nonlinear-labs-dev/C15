#pragma once
class Settings;

class SettingsUseCases
{
 public:
  explicit SettingsUseCases(Settings* s);
  void setMappingsToHighRes();
  void setMappingsToClassicMidi();

 private:
  Settings* m_settings;
};
