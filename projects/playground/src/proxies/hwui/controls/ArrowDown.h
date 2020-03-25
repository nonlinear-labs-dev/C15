#pragma once

#include "ButtonMenuButton.h"

class ArrowDown : public ButtonMenuButton
{
 private:
  typedef ButtonMenuButton super;

 public:
  explicit ArrowDown(const Rect& pos);
  ~ArrowDown() override;

 private:
  ArrowDown(const ArrowDown& other);
  ArrowDown& operator=(const ArrowDown&);

  std::shared_ptr<Font> getFont() const override;
};
