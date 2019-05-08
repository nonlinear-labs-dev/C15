#pragma once

#include <proxies/hwui/controls/LabelRegular8.h>

class DirectLoadIndicator : public LabelRegular8
{
 private:
  typedef LabelRegular8 base_type;

 public:
  DirectLoadIndicator(const Rect &pos);
  virtual ~DirectLoadIndicator();

  enum class Mode
  {
    Off,
    DirectLoad,
    Load
  };

  void setMode(Mode m);

 private:
  bool redraw(FrameBuffer &fb);
  void drawArrow(FrameBuffer &fb) const;
  void drawDirectLoad(FrameBuffer &fb);

  Mode m_mode = Mode::Off;
};
