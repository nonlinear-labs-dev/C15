#pragma once

#include <proxies/hwui/buttons.h>
#include "Carousel.h"
#include <nltools/Types.h>

class Application;
class Parameter;
class PanelUnitParameterEditMode;

class ParameterCarousel : public Carousel
{
 private:
  typedef Carousel super;

 public:
  explicit ParameterCarousel(const Rect &pos);
  ~ParameterCarousel() override;

  ParameterCarousel(const ParameterCarousel &other) = delete;
  ParameterCarousel &operator=(const ParameterCarousel &) = delete;

  void turn() override;
  void antiTurn() override;

 protected:
  void setup(Parameter *selectedParameter) override;

 private:
  using tPanelParameterEditMode = std::shared_ptr<PanelUnitParameterEditMode>;

  void setupChildControls(const tPanelParameterEditMode &edit, Parameter *selectedParameter, Buttons button);
  void setupChildControls(Parameter *selectedParameter, const std::__cxx11::list<int> &buttonAssignments);
  void setupChildControlsForParameterWithoutButtonMapping(Parameter *selectedParameter);
};
