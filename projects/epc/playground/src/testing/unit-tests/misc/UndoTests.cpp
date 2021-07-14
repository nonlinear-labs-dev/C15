#include <testing/TestHelper.h>
#include <libundo/undo/TransactionLog.h>
#include <list>

using namespace std::chrono_literals;

TEST_CASE("Timestamped Undo")
{
  auto now = std::chrono::system_clock::time_point();
  std::list<std::chrono::seconds> timestamps { 1s, 2s, 3s, 4s, 5s };

  auto getTime = [&]() {
    auto offset = timestamps.front();
    timestamps.pop_front();
    return now + offset;
  };

  UNDO::TransactionLog log(getTime);
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(1));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(3));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(4));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(5));

  CHECK(log.findRecentTransactionAt(now + 1s) == reinterpret_cast<const UNDO::Transaction *>(1));
  CHECK(log.findRecentTransactionAt(now + 2s) == reinterpret_cast<const UNDO::Transaction *>(1));
  CHECK(log.findRecentTransactionAt(now + 3s) == reinterpret_cast<const UNDO::Transaction *>(2));
  CHECK(log.findRecentTransactionAt(now + 4s) == reinterpret_cast<const UNDO::Transaction *>(3));
  CHECK(log.findRecentTransactionAt(now + 5s) == reinterpret_cast<const UNDO::Transaction *>(4));
  CHECK(log.findRecentTransactionAt(now + 10s) == reinterpret_cast<const UNDO::Transaction *>(5));
}

TEST_CASE("Timestamped Undo - find after undo")
{
  auto now = std::chrono::system_clock::time_point();
  std::list<std::chrono::seconds> timestamps { 1s, 2s, 3s, 4s, 5s };

  auto getTime = [&]() {
    auto offset = timestamps.front();
    timestamps.pop_front();
    return now + offset;
  };

  UNDO::TransactionLog log(getTime);
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(1));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(3));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));  // undo step 2
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(3));  // undo step 3

  CHECK(log.findRecentTransactionAt(now + 1s) == reinterpret_cast<const UNDO::Transaction *>(1));
  CHECK(log.findRecentTransactionAt(now + 2s) == reinterpret_cast<const UNDO::Transaction *>(1));
  CHECK(log.findRecentTransactionAt(now + 3s) == reinterpret_cast<const UNDO::Transaction *>(2));
  CHECK(log.findRecentTransactionAt(now + 4s) == reinterpret_cast<const UNDO::Transaction *>(3));
  CHECK(log.findRecentTransactionAt(now + 5s) == reinterpret_cast<const UNDO::Transaction *>(2));
  CHECK(log.findRecentTransactionAt(now + 10s) == reinterpret_cast<const UNDO::Transaction *>(3));
}

TEST_CASE("Timestamped Undo - duplicate")
{
  auto now = std::chrono::system_clock::time_point();
  std::list<std::chrono::seconds> timestamps { 1s, 2s, 3s, 4s, 5s };

  auto getTime = [&]() {
    auto offset = timestamps.front();
    timestamps.pop_front();
    return now + offset;
  };

  UNDO::TransactionLog log(getTime);
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(1));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));

  CHECK(timestamps.size() == 3);  // only 2 items popped
}

TEST_CASE("Timestamped Undo - remove")
{
  auto now = std::chrono::system_clock::time_point();
  std::list<std::chrono::seconds> timestamps { 1s, 2s, 3s, 4s, 5s };

  auto getTime = [&]() {
    auto offset = timestamps.front();
    timestamps.pop_front();
    return now + offset;
  };

  UNDO::TransactionLog log(getTime);
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(1));
  log.addEntryIfChanged(reinterpret_cast<const UNDO::Transaction *>(2));
  CHECK(log.getSize() == 2);
  log.removeTransaction(reinterpret_cast<const UNDO::Transaction *>(1));
  CHECK(log.getSize() == 1);
  log.removeTransaction(reinterpret_cast<const UNDO::Transaction *>(2));
  CHECK(log.getSize() == 0);
}
