#pragma once

#include "ButtonMenuButton.h"

class ArrowUp : public ButtonMenuButton
{
 private:
  typedef ButtonMenuButton super;

 public:
  ArrowUp(const Rect& pos);
  virtual ~ArrowUp();

 private:
  // private methods
  ArrowUp(const ArrowUp& other);
  ArrowUp& operator=(const ArrowUp&);

  shared_ptr<Font> getFont() const override;
};
