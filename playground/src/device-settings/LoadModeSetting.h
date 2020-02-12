#pragma once

#include <nltools/enums/EnumTools.h>
#include <nltools/Types.h>
#include "EnumSetting.h"

ENUM(LoadMode, uint8_t, LoadToPart, Select, DirectLoad);

class LoadModeSetting : public EnumSetting<LoadMode>
{
 private:
  typedef EnumSetting<LoadMode> super;

 public:
  explicit LoadModeSetting(Settings& settings);

  Glib::ustring getDisplayStringForVoiceGroup(VoiceGroup vg) const;
  Glib::ustring getDisplayStringForVoiceGroup(VoiceGroup vg, LoadMode setting) const;

  LoadModeSetting(const LoadModeSetting& other) = delete;
  LoadModeSetting& operator=(const LoadModeSetting&) = delete;

  const std::vector<ustring>& enumToString() const override;
  const std::vector<ustring>& enumToDisplayString() const override;

  void cycleForSoundType(SoundType type);
  void antiCycleForSoundType(SoundType type);

 private:
  std::array<LoadMode, 2> m_singleSoundOrder;
  std::array<LoadMode, 3> m_dualSoundOrder;
  template <typename T> void updateLoadMode(int current, const T& loadModeOrder);
};
