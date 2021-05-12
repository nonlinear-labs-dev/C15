#include <catch.hpp>
#include <nltools/threading/ContextBoundMessageQueue.h>
#include <glibmm.h>
#include <future>

TEST_CASE("ContextBoundMessageQueue - happy path")
{
  bool received = false;
  auto ctx = Glib::MainContext::create();
  nltools::threading::ContextBoundMessageQueue queue(ctx);
  auto fn = [&] { received = true; };
  queue.pushMessage(fn);

  REQUIRE(queue.isPending());
  REQUIRE(ctx->iteration(true));
  REQUIRE(received);
  REQUIRE(!queue.isPending());
}

TEST_CASE("ContextBoundMessageQueue - happy path, multiple callbacks")
{
  int cnt = 0;
  auto ctx = Glib::MainContext::create();
  nltools::threading::ContextBoundMessageQueue queue(ctx);
  auto fn = [&] { cnt++; };
  queue.pushMessage(fn);
  queue.pushMessage(fn);
  REQUIRE(queue.isPending());
  REQUIRE(ctx->iteration(true));
  REQUIRE(cnt == 2);
  REQUIRE(!queue.isPending());
}

TEST_CASE("ContextBoundMessageQueue - destruction while pending")
{
  bool received = false;
  auto ctx = Glib::MainContext::create();
  auto queue = std::make_unique<nltools::threading::ContextBoundMessageQueue>(ctx);
  auto fn = [&] { received = true; };
  queue->pushMessage(fn);
  REQUIRE(queue->isPending());
  queue.reset();
  REQUIRE(ctx->iteration(true));
  REQUIRE(!received);
}

TEST_CASE("ContextBoundMessageQueue - multi threading")
{
  using namespace std::chrono_literals;

  std::atomic<bool> closed { false };
  auto ctx = Glib::MainContext::create();
  auto loop = Glib::MainLoop::create(ctx);
  auto runner = std::async(std::launch::async, [&] { loop->run(); });

  auto queue = std::make_unique<nltools::threading::ContextBoundMessageQueue>(ctx);

  for(int i = 0; i < 1000; i++)
  {
    queue->pushMessage([&] {
      std::this_thread::sleep_for(1ms);
      REQUIRE_FALSE(closed);
    });
  }

  queue.reset();
  closed = true;
  std::this_thread::sleep_for(1ms);
  loop->quit();
}
