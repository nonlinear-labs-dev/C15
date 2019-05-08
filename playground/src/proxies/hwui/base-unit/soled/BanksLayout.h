#pragma once

#include <proxies/hwui/DFBLayout.h>

class Label;
class PresetBank;
class Setting;
class DirectLoadIndicator;

class BanksLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  BanksLayout();
  virtual ~BanksLayout();

 private:
  void onBankSelected(shared_ptr<PresetBank> bank);
  void onAutoLoadSettingChanged(const Setting *s);

  Label *m_number;
  Label *m_name;
  DirectLoadIndicator *m_directLoad;
};
