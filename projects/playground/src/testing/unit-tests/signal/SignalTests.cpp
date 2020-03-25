#include <testing/TestHelper.h>

namespace Detail
{
  class Sender : public sigc::trackable
  {
   public:
    Signal<void, bool> sig;
  };

  class Receiver : public sigc::trackable
  {
   public:
    Receiver(int &r)
        : received(r)
    {
    }
    void fn(bool v)
    {
      received++;
    }

    int &received;
  };
}

TEST_CASE("Normal life cycle", "[Signals]")
{
  int received = 0;
  auto *s = new Detail::Sender();
  auto *r = new Detail::Receiver(received);
  s->sig.connectAndInit(mem_fun(r, &Detail::Receiver::fn), false);
  g_main_iteration(FALSE);
  delete r;
  delete s;
  CHECK(received == 1);
}

TEST_CASE("Sender dies first", "[Signals]")
{

  int received = 0;
  auto *s = new Detail::Sender();
  auto *r = new Detail::Receiver(received);
  s->sig.connectAndInit(mem_fun(r, &Detail::Receiver::fn), false);
  delete s;
  g_main_iteration(FALSE);
  delete r;
  CHECK(received == 0);
}

TEST_CASE("Receiver Dies First", "[Signals]")
{
  int received = 0;
  auto *s = new Detail::Sender();
  auto *r = new Detail::Receiver(received);
  s->sig.connectAndInit(mem_fun(r, &Detail::Receiver::fn), false);
  delete r;
  g_main_iteration(FALSE);
  delete s;
  CHECK(received == 0);
}

TEST_CASE("Reconnect Before Init CB Was Received", "[Signals]")
{
  class Sender : public sigc::trackable
  {
   public:
    Signal<void, Sender *> sig;
  };

  class Receiver : public sigc::trackable
  {
   public:
    Receiver()
    {
    }
    void fn(const Sender *v)
    {
      received[v]++;
    }

    std::map<const Sender *, int> received;
  };

  auto senderA = std::make_unique<Sender>();
  auto senderB = std::make_unique<Sender>();
  auto r = std::make_unique<Receiver>();

  auto connection = senderA->sig.connectAndInit(mem_fun(r.get(), &Receiver::fn), senderA.get());
  connection.disconnect();
  connection = senderB->sig.connectAndInit(mem_fun(r.get(), &Receiver::fn), senderB.get());
  g_main_iteration(FALSE);
  CHECK(r->received[senderA.get()] == 0);
  CHECK(r->received[senderB.get()] == 1);
}