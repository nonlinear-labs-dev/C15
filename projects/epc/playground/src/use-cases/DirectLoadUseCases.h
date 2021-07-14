#pragma once
#include <device-settings/DirectLoadSetting.h>

class HWUI;

class DirectLoadUseCases
{
 public:
  explicit DirectLoadUseCases(DirectLoadSetting* setting);

  void toggleDirectLoadFromHWUI(HWUI* pHwui);
  void toggleDirectLoadFromBaseUnit();
  void enableDirectLoadFromWebUI(Preset* pPreset, VoiceGroup from, VoiceGroup to);
  void disableDirectLoad();

  void setDirectLoad(bool b);

  void enableDirectLoadWithoutPreset();

 private:
  DirectLoadSetting* m_setting;
};