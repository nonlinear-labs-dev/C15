#pragma once

#include "proxies/hwui/controls/Label.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class LockedIndicator : public Label
{
 private:
  typedef Label super;

 public:
  LockedIndicator(const Rect &pos);
  virtual ~LockedIndicator();

 protected:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);

  virtual void onParameterGroupChanged();

  sigc::connection m_groupConnection;
};
