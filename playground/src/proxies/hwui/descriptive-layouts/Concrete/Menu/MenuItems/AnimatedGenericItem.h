#pragma once

#include <nltools/logging/Log.h>
#include "GenericItem.h"

class AnimatedGenericItem : public GenericItem
{
 public:
  template <class tCap, class tCB>
  AnimatedGenericItem(tCap caption, tCB cb)
      : GenericItem(caption, cb)
  {
  }

  ~AnimatedGenericItem() override
  {
    m_signal.disconnect();
  }

  void startAnimation()
  {
    m_signal.disconnect();

    m_animationPercent = 0.0;
    int interval = static_cast<int>(m_animationLenght) / c_animationSteps;

    m_signal = Application::get().getMainContext()->signal_timeout().connect(
        sigc::mem_fun(this, &AnimatedGenericItem::doAnimation), interval);
  }

  bool doAnimation()
  {
    if(m_animationPercent < static_cast<float>(m_animationLenght))
    {
      m_animationPercent += c_animationSteps;
      setDirty();
      return true;
    }
    else
    {
      m_animationPercent = 0.0f;
      setDirty();
      return false;
    }
  }

  bool drawAnimationZug(FrameBuffer &buffer)
  {
    auto pos = getPosition();

    auto percent = m_animationPercent / static_cast<float>(m_animationLenght);

    pos.setWidth(static_cast<int>(pos.getWidth() * percent));
    buffer.setColor(FrameBuffer::C255);
    buffer.fillRect(pos);
    return false;
  }

  bool redraw(FrameBuffer &fb) override
  {
    auto ret = ControlWithChildren::redraw(fb);
    ret |= drawAnimationZug(fb);
    ret |= drawHighlightBorder(fb);
    return ret;
  }

  void doAction() override
  {
    GenericItem::doAction();
    startAnimation();
  }

  template <class tCB> void onAnimationCompleted(tCB cb)
  {
    m_animationFinishedCB = cb;
  }

 protected:
  sigc::connection m_signal;

  float m_animationPercent = { 0.0 };
  int m_animationLenght = 1500;
  static constexpr auto c_animationSteps = 100;
  std::function<void(void)> m_animationFinishedCB;
};
