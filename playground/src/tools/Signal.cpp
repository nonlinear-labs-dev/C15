#include "Signal.h"
#include <testing/TestDriver.h>
#include <device-settings/DebugLevel.h>

static TestDriver<Signal<void, bool>> tests;

template <> void Signal<void, bool>::registerTests()
{
  class Sender : public sigc::trackable
  {
   public:
    Signal<void, bool> sig;
  };

  class Receiver : public sigc::trackable
  {
   public:
    Receiver(bool &r)
        : received(r)
    {
    }
    void fn(bool v)
    {
      received = true;
    }

    bool &received;
  };

  g_test_add_func("/Signals/normalLifeCycle", []() {
    bool received = false;
    Sender *s = new Sender();
    Receiver *r = new Receiver(received);
    s->sig.connectAndInit(mem_fun(r, &Receiver::fn), false);
    g_main_iteration(FALSE);
    delete r;
    delete s;
    g_assert(received);
  });

  g_test_add_func("/Signals/senderDiesFirst", []() {
    bool received = false;
    Sender *s = new Sender();
    Receiver *r = new Receiver(received);
    s->sig.connectAndInit(mem_fun(r, &Receiver::fn), false);
    delete s;
    g_main_iteration(FALSE);
    delete r;
    g_assert(!received);
  });

  g_test_add_func("/Signals/receiverDiesFirst", []() {
    bool received = false;
    Sender *s = new Sender();
    Receiver *r = new Receiver(received);
    s->sig.connectAndInit(mem_fun(r, &Receiver::fn), false);
    delete r;
    g_main_iteration(FALSE);
    delete s;
    g_assert(!received);
  });
}
