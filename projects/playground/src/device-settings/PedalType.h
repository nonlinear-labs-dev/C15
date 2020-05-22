#pragma once

#include <nltools/enums/EnumTools.h>
#include <EHC-pedal-presets.h>
#include "EnumSetting.h"

ENUM(PedalTypes, int, PotTipActive = EHC_PRESET_Pot_Tip_Active, PotRingActive = EHC_PRESET_Pot_Ring_Active,
     PotTipActiveReverse = EHC_PRESET_Pot_Tip_Act_Rev, PotRingActiveReverse = EHC_PRESET_Pot_Ring_Act_Rev,
     Resitor = EHC_PRESET_Resistor, ResistorReversed = EHC_PRESET_Resistor_Rev,
     SwitchClosing = EHC_PRESET_Switch_Closing, SwitchOpening = EHC_PRESET_Switch_Opening, CV0to5V = EHC_PRESET_CV_0_5V,
     CV0To5VAutoRange = EHC_PRESET_CV_0_5V_Auto_Range, OFF = EHC_PRESET_OFF, BossEV30 = EHC_PRESET_Boss_EV_30,
     BossFV500L = EHC_PRESET_Boss_FV_500L, DoepferFP5 = EHC_PRESET_Doepfer_FP5, FractalEV2 = EHC_PRESET_Fractal_EV_2,
     KorgDS1H = EHC_PRESET_Korg_DS_1H, KorgEXP2 = EHC_PRESET_Korg_EXP_2, LeadFootLFX1 = EHC_PRESET_Lead_Foot_LFX_1,
     MAudioEXP = EHC_PRESET_M_Audio_EX_P_M, MoogEP3 = EHC_PRESET_Moog_EP_3_std,
     RolandDP10 = EHC_PRESET_Roland_DP_10_cont, RolandEV5 = EHC_PRESET_Roland_EV_5, YamahaFC3A = EHC_PRESET_Yamaha_FC3A,
     YamahaFC7 = EHC_PRESET_Yamaha_FC7)

class PedalType : public EnumSetting<PedalTypes>
{
 private:
  typedef EnumSetting<PedalTypes> super;

 public:
  PedalType(UpdateDocumentContributor &settings, uint16_t lpcKey);
  virtual ~PedalType();

  const std::vector<Glib::ustring> &enumToDisplayString() const override;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void load(const Glib::ustring &text) override;

 private:
  void sendToLPC() const override;
  const std::vector<Glib::ustring> &enumToString() const override;

  uint16_t m_lpcKey;
};
