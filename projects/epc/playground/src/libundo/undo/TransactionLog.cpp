#include "TransactionLog.h"
#include <algorithm>

namespace UNDO
{

  TransactionLog::TransactionLog(TimeGetter getTime)
      : m_getTime(getTime)
  {
  }

  void TransactionLog::addEntryIfChanged(const UNDO::Transaction *transaction)
  {
    if(m_entries.empty() || m_entries.back().second != transaction)
      m_entries.emplace_back(m_getTime(), transaction);
  }

  void TransactionLog::removeTransaction(const Transaction *transaction)
  {
    auto end = std::remove_if(m_entries.begin(), m_entries.end(),
                              [&](const auto &entry) { return entry.second == transaction; });
    m_entries.erase(end, m_entries.end());
  }

  size_t TransactionLog::getSize() const
  {
    return m_entries.size();
  }

  const Transaction *TransactionLog::findRecentTransactionAt(TimePoint time) const
  {
    auto key = std::make_pair(time, static_cast<Transaction *>(nullptr));
    auto it = std::lower_bound(m_entries.begin(), m_entries.end(), key,
                               [](const auto &a, const auto b) { return a.first < b.first; });

    if(it != m_entries.end())
    {
      if(it != m_entries.begin())
        return std::prev(it)->second;

      return it->second;
    }

    return m_entries.empty() ? nullptr : m_entries.back().second;
  }
}