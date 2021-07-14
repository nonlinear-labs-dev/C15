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
  explicit LockedIndicator(const Rect &pos);
  ~LockedIndicator() override;

 protected:
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  void onParameterSelected(Parameter *newOne);

  virtual void onParameterGroupChanged();

  sigc::connection m_groupConnection;
};
