#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>

class Parameter;

class AboutText : public MultiLineLabel, public Scrollable
{
 private:
  typedef MultiLineLabel super;

 public:
  AboutText();

  virtual void setPosition(const Rect &rect) override;
  virtual const Rect &getPosition() const override;
  virtual void setDirty() override;
};
