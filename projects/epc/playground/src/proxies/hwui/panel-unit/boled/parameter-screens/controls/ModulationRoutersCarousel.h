#pragma once

#include <proxies/hwui/controls/ButtonMenu.h>
#include "Carousel.h"

class Application;
class Parameter;

class ModulationRoutersCarousel : public Carousel
{
 private:
  typedef Carousel super;

 public:
  explicit ModulationRoutersCarousel(const Rect &pos);
  ~ModulationRoutersCarousel() override;

  ModulationRoutersCarousel(const ModulationRoutersCarousel &other) = delete;
  ModulationRoutersCarousel &operator=(const ModulationRoutersCarousel &) = delete;

  void turn() override;
  void antiTurn() override;
  void setHighlight(bool isHighlight) override;

 protected:
  void setup(Parameter *selectedParameter) override;
};
