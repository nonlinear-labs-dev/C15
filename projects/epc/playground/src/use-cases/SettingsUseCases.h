#pragma once
class Settings;

class SettingsUseCases
{
 public:
  explicit SettingsUseCases(Settings* s);
  void setMappingsToHighRes();
  void setMappingsToClassicMidi();

  void updateRoutingAspect(int entry, int aspect, bool value);
  static void panicAudioEngine();

  void setAllRoutingEntries(bool state);

 private:
  Settings* m_settings;
};
