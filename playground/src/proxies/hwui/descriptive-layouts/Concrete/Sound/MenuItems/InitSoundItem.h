#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/AbstractMenuItems/BasicItem.h>

class InitSound : public BasicItem
{
 public:
  InitSound();
  void doAction() override;
};