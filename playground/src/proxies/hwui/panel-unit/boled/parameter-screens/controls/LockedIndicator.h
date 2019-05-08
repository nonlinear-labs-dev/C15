#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class LockedIndicator : public Label
{
 private:
  typedef Label super;

 public:
  LockedIndicator(const Rect &pos);
  virtual ~LockedIndicator();

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);
  void onParameterGroupChanged();

  sigc::connection m_groupConnection;
};
