#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;

class NumBanksLabel : public Label
{
 private:
  typedef Label super;

 public:
  NumBanksLabel(const Rect& pos);
  virtual ~NumBanksLabel();

 private:
  void onNumBanksChanged(int numBanks);
};
