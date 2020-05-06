#pragma once

#include <nltools/enums/EnumTools.h>
#include "EnumSetting.h"

ENUM(PedalTypes, int, PotTipActive = 0, PotRingActive = 1, PotTipActiveReverse = 2, PotRingActiveReverse = 3,
     Resitor = 4, ResistorReversed = 5, SwitchClosing = 6, SwitchOpening = 7, CV0to5V = 8, CV0To5VAutoRange = 9,
     OFF = 10, BossEV30 = 11, BossFV500L = 12, DoepferFP5 = 13, FractalEV2 = 14, KorgDS1H = 15, KorgEXP2 = 16,
     LeadFootLFX1 = 17, MAudioEXP = 18, MoogEP3 = 19, RolandDP10 = 20, RolandEV5 = 21, YamahaFC3A = 22, YamahaFC7 = 23)

class PedalType : public EnumSetting<PedalTypes>
{
 private:
  typedef EnumSetting<PedalTypes> super;

 public:
  PedalType(UpdateDocumentContributor &settings, uint16_t lpcKey);
  virtual ~PedalType();

  const std::vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  void sendToLPC() const override;
  const std::vector<Glib::ustring> &enumToString() const override;

  uint16_t m_lpcKey;
};
