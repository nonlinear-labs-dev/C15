#pragma once

#include "GenericItem.h"
#include <sigc++/connection.h>
#include <chrono>

class Animator
{
 public:
  using ProgressCB = std::function<void()>;
  using FinishedCB = std::function<void()>;

  Animator(std::chrono::milliseconds length, ProgressCB pcb, FinishedCB fcb);
  virtual ~Animator();
  float getAnimationPosition() const;

 private:
  bool doAnimation();
  bool finishAnimation();

  sigc::connection m_signal;
  ProgressCB m_animationCB;
  FinishedCB m_animationFinishedCB;

  std::chrono::steady_clock::time_point m_animationStartedAt = std::chrono::steady_clock::now();
  std::chrono::milliseconds m_animationLength { 500 };
};

class AnimatedGenericItem : public GenericItem
{
 public:
  template <class tCap, class tCB>
  AnimatedGenericItem(tCap caption, const Rect &r, tCB cb)
      : GenericItem(caption, r, [] {})
  {
    m_animationFinishedCB = cb;
  }

  void startAnimation();
  bool drawAnimationZug(FrameBuffer &buffer);
  bool redraw(FrameBuffer &fb) override;
  void doAction() override;

 private:
  std::unique_ptr<Animator> m_animator;
  std::function<void(void)> m_animationFinishedCB;
};
