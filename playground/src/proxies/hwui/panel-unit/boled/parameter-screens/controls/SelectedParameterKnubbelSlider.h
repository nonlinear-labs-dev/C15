#pragma once

#include "SelectedParameterDotSlider.h"

class Application;

class SelectedParameterKnubbelSlider : public SelectedParameterDotSlider
{
 private:
  typedef SelectedParameterDotSlider super;

 public:
  SelectedParameterKnubbelSlider(const Rect &rect);
  virtual ~SelectedParameterKnubbelSlider();

 protected:
  virtual void drawHandle(FrameBuffer &fb) override;

 private:
  SelectedParameterKnubbelSlider(const SelectedParameterKnubbelSlider &other);
  SelectedParameterKnubbelSlider &operator=(const SelectedParameterKnubbelSlider &);
};
