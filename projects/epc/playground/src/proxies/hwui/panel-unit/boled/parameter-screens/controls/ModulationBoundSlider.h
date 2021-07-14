#pragma once

#include <proxies/hwui/controls/Control.h>

class ModulateableParameter;
class Parameter;

class ModulationBoundSlider : public Control
{
  typedef Control super;

 public:
  explicit ModulationBoundSlider(const Rect &pos);
  ~ModulationBoundSlider() override;

  bool redraw(FrameBuffer &fb) override;

 protected:
  enum class BoundPosition
  {
    Left,
    Right
  };

  virtual BoundPosition getBoundPosition() const = 0;
  void onSelectionChanged(Parameter *, Parameter *newParam);
  void onParameterChanged(const Parameter *);

  ModulateableParameter *m_param = nullptr;
  sigc::connection m_paramChanged;
};
