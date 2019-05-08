#pragma once

#include "ButtonMenuButton.h"

class ArrowDown : public ButtonMenuButton
{
 private:
  typedef ButtonMenuButton super;

 public:
  ArrowDown(const Rect& pos);
  virtual ~ArrowDown();

 private:
  // private methods
  ArrowDown(const ArrowDown& other);
  ArrowDown& operator=(const ArrowDown&);

  shared_ptr<Font> getFont() const override;
};
