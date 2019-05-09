#pragma once

#include "LabelRegular8.h"
#include <proxies/hwui/buttons.h>

class TextEditUsageMode;

class RenameLetter : public LabelRegular8
{
 private:
  typedef LabelRegular8 super;

 public:
  RenameLetter(std::shared_ptr<TextEditUsageMode> mode, Buttons buttonID, const Rect &pos);

  StringAndSuffix getText() const override;
  bool redraw(FrameBuffer &fb) override;

 private:
  int getXOffset() const override;
  bool highlightLetter() const;

  std::shared_ptr<TextEditUsageMode> m_mode;
  Buttons m_buttonID;
};
