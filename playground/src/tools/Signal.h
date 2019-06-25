#pragma once

#include "playground.h"
#include <atomic>
#include <sigc++/signal.h>
#include <type_traits>
#include <tuple>

using namespace Glib;
using namespace sigc;

template <typename tFirst, typename... tArgs> class Signal : public sigc::signal<tFirst, tArgs...>
{
 private:
  typedef sigc::signal<tFirst, tArgs...> super;

 public:
  Signal()
      : m_initCallbackScheduled(false)
  {
  }

  virtual ~Signal()
  {
    m_initCallbackConnection.disconnect();
    m_deferedCallbackConnection.disconnect();
  }

  sigc::connection connectAndInit(const typename super::slot_type &slot, const tArgs &... args)
  {
    auto cb = std::bind(&super::slot_type::operator(), slot, args...);
    auto ret = super::connect(slot);
    const auto &addedSlot = *this->slots().rbegin();
    const void *cookie = &addedSlot;
    scheduleInitCallback(cookie, cb);
    return ret;
  }

  typename super::result_type send(tArgs... args)
  {
    m_initRecords.clear();
    return this->emit(args...);
  }

  void deferedSend(const tArgs &... args)
  {
    m_deferedSend = [=]() { send(args...); };

    if(!m_deferedCallbackConnection.connected())
    {
      auto ctx = MainContext::get_default();
      m_deferedCallbackConnection
          = ctx->signal_idle().connect(mem_fun(this, &Signal::emitDefered), Glib::PRIORITY_DEFAULT);
    }
  }

  static void registerTests();

 private:
  Signal(const Signal &other);
  Signal &operator=(const Signal &);

  typedef std::function<void()> tCallback;
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

  using super::emit;
  using super::operator();

  void scheduleInitCallback(const void *cookie, tCallback cb)
  {
    m_initRecords.push_back({ cb, cookie });

    if(!m_initCallbackScheduled.exchange(true))
    {
      m_initCallbackConnection = MainContext::get_default()->signal_idle().connect(
          mem_fun(this, &Signal::doTheCallbacks), Glib::PRIORITY_HIGH);
    }
  }

  bool doTheCallbacks()
  {
    m_initCallbackScheduled.exchange(false);

    tInitRecords initRecords;
    swap(initRecords, m_initRecords);

    m_initCallbackConnection.disconnect();

    for(const auto &r : initRecords)
      for(auto &s : this->slots())
        if(&s == r.cookie)
          r.cb();

    return false;
  }

  bool emitDefered()
  {
    m_deferedCallbackConnection.disconnect();

    tCallback c;
    std::swap(c, m_deferedSend);

    if(c)
      c();

    return false;
  }
};

template <typename tFirst, typename... tArgs> class SignalWithCache : public Signal<tFirst, tArgs...>
{
 public:
  using super = Signal<tFirst, tArgs...>;
  using super::super;

  typename super::result_type send(tArgs... args)
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
