#include "AnimationControl.h"
#include <Application.h>

AnimationControl::AnimationControl(const Rect& r, int animationLenght, int animationSteps)
    : Control(r)
    , m_animationLenght{ animationLenght }
    , m_animationSteps{ animationSteps }
{
}

AnimationControl::~AnimationControl() {
  m_signal.disconnect();
}

bool AnimationControl::doAnimationWrapper()
{
  m_currentStep += m_animationLenght / m_animationSteps;

  if(doAnimation(static_cast<float>(m_currentStep) / static_cast<float>(m_animationLenght)))
    return true;

  if(m_animationFinishedCB)
    m_animationFinishedCB();

  setDirty();
  return false;
}

void AnimationControl::startAnimation(const std::function<void(void)>& onAnimFinishedCB)
{
  m_signal.disconnect();

  m_currentStep = 0;
  int interval = static_cast<int>(m_animationLenght) / m_animationSteps;

  m_animationFinishedCB = onAnimFinishedCB;

  m_signal = Application::get().getMainContext()->signal_timeout().connect(
      sigc::mem_fun(this, &AnimationControl::doAnimationWrapper), interval);
}
