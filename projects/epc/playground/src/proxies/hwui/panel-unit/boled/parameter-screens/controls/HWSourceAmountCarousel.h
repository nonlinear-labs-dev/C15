#pragma once

#include "Carousel.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class HWSourceAmountCarousel : public Carousel
{
 private:
  typedef Carousel super;

 public:
  explicit HWSourceAmountCarousel(const Rect& pos);
  HWSourceAmountCarousel(const HWSourceAmountCarousel& other) = delete;
  HWSourceAmountCarousel& operator=(const HWSourceAmountCarousel&) = delete;

  ~HWSourceAmountCarousel() override;

  void turn() override;
  void antiTurn() override;
  void setHighlight(bool isHighlight) override;
  void highlightSelected();

 protected:
  void setup(Parameter* selectedParameter) override;

 private:
  void onMacroControlChanged(const Parameter* param);

  sigc::connection m_mcConnection;
};
