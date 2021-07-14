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
  explicit MCAmountLabel(const Rect &rect);
  ~MCAmountLabel() override;

 private:
  MCAmountLabel(const MCAmountLabel &other);
  MCAmountLabel &operator=(const MCAmountLabel &);

  void onParameterSelected(Parameter *newParameter);
  void onModifiersChanged();
  void update(const Parameter *parameter);
  void setSuffixFontColor(FrameBuffer &fb) const override;

  sigc::connection m_paramValueConnection;
};
