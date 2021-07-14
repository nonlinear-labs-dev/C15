#pragma once

#include <proxies/hwui/Layout.h>
#include <sigc++/connection.h>

class Label;
class Bank;
class Setting;
class DirectLoadIndicator;
class Uuid;

class BanksLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  BanksLayout();
  ~BanksLayout() override;

 private:
  void updateFromBank(const Bank *bank) const;
  void onBankSelected(const Uuid &selectedBank);
  void onAutoLoadSettingChanged(const Setting *s);
  void onBankChanged();

  Label *m_number;
  Label *m_name;
  Label *m_midiSymbol;
  sigc::connection m_bankconnection;
  DirectLoadIndicator *m_directLoad;
  Bank *m_bank = nullptr;
};
