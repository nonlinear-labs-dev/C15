#pragma once

#include "ButtonMenuButton.h"

class ArrowUp : public ButtonMenuButton
{
 private:
  typedef ButtonMenuButton super;

 public:
  explicit ArrowUp(const Rect& pos);
  ~ArrowUp() override;

 private:
  ArrowUp(const ArrowUp& other);
  ArrowUp& operator=(const ArrowUp&);

  std::shared_ptr<Font> getFont() const override;
};
