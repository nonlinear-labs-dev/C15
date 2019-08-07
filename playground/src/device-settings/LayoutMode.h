#pragma once
#include "Settings.h"
#include "EnumSetting.h"
#include <proxies/hwui/HWUIEnums.h>

class LayoutMode : public EnumSetting<LayoutVersionMode>
{
 public:
  explicit LayoutMode(Settings*);
  const std::vector<ustring>& enumToString() const override;
  const std::vector<ustring>& enumToDisplayString() const override;
};
