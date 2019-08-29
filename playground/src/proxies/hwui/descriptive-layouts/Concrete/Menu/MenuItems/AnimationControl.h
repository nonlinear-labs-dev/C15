#pragma once
#include <proxies/hwui/controls/Control.h>

class AnimationControl : public Control
{
 public:
  AnimationControl(const Rect& r, int animationLenght, int animationSteps);
  ~AnimationControl() override;
  void startAnimation(const std::function<void(void)>& onAnimationFinished);

  virtual bool doAnimation(float percent) = 0;

private:
  bool doAnimationWrapper();
  sigc::connection m_signal;

  int m_currentStep;
  int m_animationLenght;
  int m_animationSteps;
  std::function<void(void)> m_animationFinishedCB;
};
