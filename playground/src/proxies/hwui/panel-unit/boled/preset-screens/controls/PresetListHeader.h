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

  void setup(shared_ptr<PresetBank> bank);

 private:
  void setupWithArrows(shared_ptr<PresetBank> bank);

  bool m_showBankArrows;
};
