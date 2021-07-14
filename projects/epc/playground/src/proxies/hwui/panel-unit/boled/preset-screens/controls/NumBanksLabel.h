#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;

class NumBanksLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit NumBanksLabel(const Rect& pos);
  ~NumBanksLabel() override;

 private:
  void onNumBanksChanged(size_t numBanks);
};
