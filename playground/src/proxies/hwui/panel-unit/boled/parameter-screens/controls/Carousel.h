#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class Application;
class Parameter;

class Carousel : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  Carousel(const Rect &pos);
  virtual ~Carousel();

  virtual void turn() = 0;
  virtual void antiTurn();
  virtual void setup(Parameter *selectedParameter) = 0;

 protected:
  Application &getApp();

 private:
  Carousel(const Carousel &other);
  Carousel &operator=(const Carousel &);
};
