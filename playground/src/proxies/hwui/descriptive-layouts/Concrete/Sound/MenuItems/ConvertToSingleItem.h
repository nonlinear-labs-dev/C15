#pragma once
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/AbstractMenuItems/BasicItem.h>

class ConvertToSingle : public BasicItem
{
public:
  ConvertToSingle();
  void doAction() override;
};