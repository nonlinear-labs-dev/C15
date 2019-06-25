#pragma once

#include <proxies/hwui/DFBLayout.h>

class Label;
class Bank;
class Setting;
class DirectLoadIndicator;
class Uuid;

class BanksLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  BanksLayout();

  virtual ~BanksLayout();

 private:
  void updateFromBank(const Bank *bank) const;
  void onBankSelected(const Uuid &selectedBank);
  void onAutoLoadSettingChanged(const Setting *s);
  void onBankChanged();

  Label *m_number;
  Label *m_name;
  sigc::connection m_bankconnection;
  DirectLoadIndicator *m_directLoad;
  Bank *m_bank = nullptr;
};
