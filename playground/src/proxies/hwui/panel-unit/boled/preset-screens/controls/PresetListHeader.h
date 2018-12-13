#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class Application;

class PresetListHeader : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  PresetListHeader(const Rect &pos, bool showBankArrows);
  virtual ~PresetListHeader();

  void setup(Bank *bank);

 private:
  void setupWithArrows(Bank *bank);

  bool m_showBankArrows;
};
