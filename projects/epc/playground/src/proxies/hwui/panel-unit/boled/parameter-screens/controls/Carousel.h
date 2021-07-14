#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class Parameter;

class Carousel : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  explicit Carousel(const Rect &pos);
  ~Carousel() override;

  virtual void turn() = 0;
  virtual void antiTurn();
  virtual void setup(Parameter *selectedParameter) = 0;

 private:
  Carousel(const Carousel &other);
  Carousel &operator=(const Carousel &);

  void setupSelected();
};
