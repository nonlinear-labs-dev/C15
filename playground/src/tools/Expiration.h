#pragma once

#include "playground.h"
#include "Uncopyable.h"
#include <chrono>

class Expiration : public Uncopyable
{
 public:
  using Callback = function<void()>;
  using Duration = std::chrono::system_clock::duration;

  Expiration(Callback cb = Callback(), Duration d = Duration::zero(), int priority = Glib::PRIORITY_DEFAULT);
  ~Expiration();

  void setCallback(Callback cb);
  void refresh(Duration d, int priority = Glib::PRIORITY_DEFAULT);
  bool isPending() const;
  void cancel();

 private:
  bool doCallback();

  Callback m_cb;
  sigc::connection m_timeout;
};
