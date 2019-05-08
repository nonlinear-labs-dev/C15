#pragma once

#include "Carousel.h"

class Application;
class Parameter;

class HWSourceAmountCarousel : public Carousel
{
 private:
  typedef Carousel super;

 public:
  HWSourceAmountCarousel(const Rect& pos);
  virtual ~HWSourceAmountCarousel();

  virtual void turn() override;
  virtual void antiTurn() override;
  void setHighlight(bool isHighlight) override;
  void highlightSelected();

 protected:
  virtual void setup(Parameter* selectedParameter) override;

 private:
  HWSourceAmountCarousel(const HWSourceAmountCarousel& other);
  HWSourceAmountCarousel& operator=(const HWSourceAmountCarousel&);

  void onMacroControlChanged(const Parameter* param);

  sigc::connection m_mcConnection;
};
