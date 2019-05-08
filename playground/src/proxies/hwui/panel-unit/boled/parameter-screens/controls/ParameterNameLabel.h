#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Parameter;

class ParameterNameLabel : public Label
{
 private:
  typedef Label super;

 public:
  ParameterNameLabel(const Rect &pos);
  virtual ~ParameterNameLabel();

 private:
  ParameterNameLabel(const ParameterNameLabel &other);
  ParameterNameLabel &operator=(const ParameterNameLabel &);

  void onParameterSelected(Parameter *param);

  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onParameterChanged(const Parameter *param);

  sigc::connection m_connection;
};
