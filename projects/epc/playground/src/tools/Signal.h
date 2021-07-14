#pragma once

#include "playground.h"
#include <atomic>
#include <sigc++/signal.h>
#include <sigc++/connection.h>
#include <tuple>
#include <functional>

class SignalBase
{
 public:
  typedef std::function<void()> tCallback;

  SignalBase();
  virtual ~SignalBase();
  virtual bool findCookie(const void *) const = 0;
  void scheduleInitCallback(const void *cookie, tCallback &&cb);
  bool emitDefered();
  bool doTheCallbacks();
  void deferedSend();

  struct Record
  {
    tCallback cb;
    const void *cookie;
  };

  typedef std::list<Record> tInitRecords;
  tInitRecords m_initRecords;
  std::atomic<bool> m_initCallbackScheduled;
  sigc::connection m_initCallbackConnection;

  tCallback m_deferedSend;
  sigc::connection m_deferedCallbackConnection;
};

template <typename tFirst, typename... tArgs> class Signal : public sigc::signal<tFirst, tArgs...>, private SignalBase
{
 private:
  typedef sigc::signal<tFirst, tArgs...> super;

 public:
  Signal() = default;
  ~Signal() override = default;
  Signal(const Signal &other) = delete;
  Signal &operator=(const Signal &) = delete;

  sigc::connection connectAndInit(const typename super::slot_type &slot, const tArgs &... args)
  {
    auto cb = std::bind(&super::slot_type::operator(), slot, args...);
    auto ret = super::connect(slot);
    const auto &addedSlot = *this->slots().rbegin();
    const void *cookie = &addedSlot;
    scheduleInitCallback(cookie, std::move(cb));
    return ret;
  }

  virtual typename super::result_type send(tArgs... args)
  {
    m_initRecords.clear();
    return this->emit(args...);
  }

  void deferedSend(const tArgs &... args)
  {
    m_deferedSend = [=]() { send(args...); };
    SignalBase::deferedSend();
  }

 private:
  bool findCookie(const void *c) const override
  {
    for(auto &s : this->slots())
      if(&s == c)
        return true;

    return false;
  }

  using super::emit;
  using super::operator();
};

template <typename tFirst, typename... tArgs> class SignalWithCache : public Signal<tFirst, tArgs...>
{
 public:
  using super = Signal<tFirst, tArgs...>;
  using super::super;

  typename super::result_type send(tArgs... args) override
  {
    auto newValue = std::make_tuple(args...);
    if(newValue != m_cache)
    {
      m_cache = newValue;
      super::send(args...);
    }
  }

 private:
  std::tuple<tArgs...> m_cache;
};
