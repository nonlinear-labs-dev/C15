#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LockedIndicator.h>

class AnyParameterLockedIndicator : public LockedIndicator
{
 public:
  explicit AnyParameterLockedIndicator(const Rect &pos);
  ~AnyParameterLockedIndicator() override = default;

 private:
  void onParameterGroupChanged() override;
};
