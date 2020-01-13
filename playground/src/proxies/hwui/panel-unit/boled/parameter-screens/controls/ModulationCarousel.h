#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Carousel.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <proxies/hwui/buttons.h>

class ModulationCarousel : public Carousel, public RotaryEncoder::Receiver
{
 private:
  typedef Carousel super;

 public:
  enum class Mode
  {
    None,
    UpperBound,
    ParameterValue,
    LowerBound
  };

  ModulationCarousel(Mode mode, const Rect &pos);

  bool onRotary(int inc, ButtonModifiers modifiers) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers);

  void setup(Parameter *selectedParameter) override;
  void turn() override;
  void antiTurn() override;

  Mode getMode() const;

 private:
  const Mode m_mode;
};
