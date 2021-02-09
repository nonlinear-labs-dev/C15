#pragma once

#include "proxies/hwui/controls/Label.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class MCAmountLabel : public Label
{
 private:
  typedef Label super;

 public:
  MCAmountLabel(const Rect &rect);
  virtual ~MCAmountLabel();

 private:
  MCAmountLabel(const MCAmountLabel &other);
  MCAmountLabel &operator=(const MCAmountLabel &);

  void onParameterSelected(Parameter *newParameter, SignalOrigin signalType);
  void onModifiersChanged();
  void update(const Parameter *parameter);
  virtual void setSuffixFontColor(FrameBuffer &fb) const override;

  sigc::connection m_paramValueConnection;
};
