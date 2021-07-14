#pragma once

#include <proxies/hwui/controls/LabelRegular8.h>

class DirectLoadIndicator : public LabelRegular8
{
 private:
  typedef LabelRegular8 base_type;

 public:
  explicit DirectLoadIndicator(const Rect &pos);
  ~DirectLoadIndicator() override;

  enum class Mode
  {
    Off,
    DirectLoad,
    Load
  };

  void setMode(Mode m);

 private:
  bool redraw(FrameBuffer &fb) override;
  void drawArrow(FrameBuffer &fb) const;
  void drawDirectLoad(FrameBuffer &fb);

  Mode m_mode = Mode::Off;
};
