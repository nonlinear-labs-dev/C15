#pragma once

#include <nltools/logging/Log.h>
#include "GenericItem.h"
#include <sigc++/connection.h>
#include <glibmm/main.h>

class Animator
{
 public:
  using ProgressCB = std::function<void()>;
  using FinishedCB = std::function<void()>;

  template <typename ProgressCB, typename FinishedCB>

  Animator(std::chrono::milliseconds length, ProgressCB pcb, FinishedCB fcb)
      : m_animationCB(pcb)
      , m_animationFinishedCB(fcb)
      , m_animationLength(length)
  {
    m_signal = Application::get().getMainContext()->signal_idle().connect(sigc::mem_fun(this, &Animator::doAnimation));
  }

  virtual ~Animator()
  {
    m_signal.disconnect();
  }

  float getAnimationPosition() const  // 0 ... 1
  {
    auto now = std::chrono::steady_clock::now();
    auto diff = now - m_animationStartedAt;
    return CLAMP(1.0f * diff / m_animationLength, 0.0f, 1.0f);
  }

 private:
  bool doAnimation()
  {
    m_animationCB();

    auto diff = std::chrono::steady_clock::now() - m_animationStartedAt;

    if(diff > m_animationLength + std::chrono::milliseconds(100))
    {
      m_signal
          = Application::get().getMainContext()->signal_idle().connect(sigc::mem_fun(this, &Animator::finishAnimation));
      return false;
    }
    return true;
  }

  bool finishAnimation()
  {
    m_animationFinishedCB();
    return false;
  }

  sigc::connection m_signal;
  ProgressCB m_animationCB;
  FinishedCB m_animationFinishedCB;

  std::chrono::steady_clock::time_point m_animationStartedAt = std::chrono::steady_clock::now();
  std::chrono::milliseconds m_animationLength{ 500 };
};

class AnimatedGenericItem : public GenericItem
{
 public:
  template <class tCap, class tCB, class tCB2>
  AnimatedGenericItem(tCap caption, const Rect &r, tCB cb, tCB2 onAnimationFinishedCB = nullptr)
      : GenericItem(caption, r, cb)
  {
    m_animationFinishedCB = onAnimationFinishedCB;
  }

  ~AnimatedGenericItem() override
  {
    m_animator.reset();
  }

  void startAnimation()
  {
    if(!m_animator)
      m_animator = std::make_unique<Animator>(std::chrono::milliseconds(500), [this] { this->setDirty(); },
                                              [this] {
                                                this->setDirty();

                                                m_animator.reset();

                                                if(m_animationFinishedCB)
                                                  m_animationFinishedCB();
                                              });
  }

  bool drawAnimationZug(FrameBuffer &buffer)
  {
    if(!m_animator)
      return false;

    auto pos = getPosition();
    auto p = m_animator->getAnimationPosition();
    float newWidth = p * static_cast<float>(pos.getWidth());
    pos.setWidth(static_cast<int>(newWidth));
    buffer.setColor(FrameBuffer::C255);
    buffer.fillRect(pos);
    return true;
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

 private:
  std::unique_ptr<Animator> m_animator;
  std::function<void(void)> m_animationFinishedCB;
};
