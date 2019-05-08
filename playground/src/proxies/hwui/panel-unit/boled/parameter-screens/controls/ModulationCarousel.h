#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Carousel.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>

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
  virtual ~ModulationCarousel();

  bool onRotary(int inc, ButtonModifiers modifiers) override;

  void setup(Parameter *selectedParameter) override;
  void turn() override;
  void antiTurn() override;
};
