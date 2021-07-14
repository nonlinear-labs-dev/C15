#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class PedalType;

class PedalView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  explicit PedalView(std::shared_ptr<PedalType> s);
  ~PedalView() override;
  bool redraw(FrameBuffer& fb) override;

 private:
  void onSettingChanged(const Setting* s);
};
