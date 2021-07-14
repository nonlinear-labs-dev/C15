#pragma once

#include <vector>
#include <chrono>
#include <functional>

namespace UNDO
{
  class Transaction;

  class TransactionLog
  {
   public:
    using TimePoint = std::chrono::system_clock::time_point;
    using TimeGetter = std::function<TimePoint()>;

    TransactionLog(TimeGetter getTime = std::chrono::system_clock::now);

    void addEntryIfChanged(const Transaction* transaction);
    void removeTransaction(const Transaction* transaction);

    [[nodiscard]] size_t getSize() const;
    [[nodiscard]] const Transaction* findRecentTransactionAt(TimePoint timestamp) const;

   private:
    using Entry = std::pair<TimePoint, const Transaction*>;
    using Entries = std::vector<Entry>;

    TimeGetter m_getTime;
    Entries m_entries;
  };

}