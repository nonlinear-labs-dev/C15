#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Carousel.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <proxies/hwui/buttons.h>
#include <tools/OnParameterChangedNotifier.h>

class Button;

class ModulationCarousel : public Carousel, public RotaryEncoder::Receiver
{
 private:
  typedef Carousel super;

 public:
  bool redraw(FrameBuffer &fb) override;

 public:
  enum class Mode
  {
    None,
    UpperBound,
    ParameterValue,
    LowerBound
  };

  ModulationCarousel(const Rect &pos);
  ModulationCarousel(Mode mode, const Rect &pos);

  bool onRotary(int inc, ButtonModifiers modifiers) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers);

  void setup(Parameter *selectedParameter) override;
  void turn() override;
  void antiTurn() override;

  void onModulationSourceChanged(const ModulateableParameter *modP);

  void forceHighlights(const ModulateableParameter* modP = nullptr);

  void drawBackground(FrameBuffer &fb) override;

  OnModulationChangedNotifier<ModulationCarousel> m_modulationNotifier;
  Button *m_button;
  Control *m_upper, *m_lower, *m_middle;
  static Mode s_lastMode;
};
