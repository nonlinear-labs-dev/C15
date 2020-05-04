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
  ParameterCarousel(const Rect &pos);
  virtual ~ParameterCarousel();

  virtual void turn() override;
  virtual void antiTurn() override;

  void requestTurn();

 protected:
  virtual void setup(Parameter *selectedParameter) override;

 private:
  ParameterCarousel(const ParameterCarousel &other);
  ParameterCarousel &operator=(const ParameterCarousel &);
  void setupChildControls(const std::shared_ptr<PanelUnitParameterEditMode> &edit, Parameter *selectedParameter,
                          Buttons button);
  void setupChildControls(Parameter *selectedParameter, const std::__cxx11::list<int> &buttonAssignments);

  void setupChildControlsForParameterWithoutButtonMapping(Parameter *selectedParameter);
};
