#pragma once

#include "Transaction.h"
#include <memory>
#include <chrono>

namespace UNDO
{
  using std::chrono::steady_clock;

  class ContinuousTransaction : public UNDO::Transaction
  {
    typedef UNDO::Transaction super;

   public:
    ContinuousTransaction(Scope &scope, void *id, const Glib::ustring &name, size_t depth);
    ~ContinuousTransaction() override;

    std::chrono::steady_clock::duration getAge() const;
    void *getID() const;
    void setIsNested();

    void setClosingCommand(std::unique_ptr<ContinuousTransaction> command);
    UNDO::ContinuousTransaction *getClosingCommand() const;

    void stopContinuation();
    bool isContinuing() const;

   protected:
    void implUndoAction() const override;
    void implRedoAction() const override;

    void onImplUndoActionStart() const override;
    void onImplRedoActionFinished() const override;

   private:
    void *m_id = nullptr;
    bool m_continuing = true;
    steady_clock::time_point m_creationTimestamp;
    std::unique_ptr<ContinuousTransaction> m_closingCommand = nullptr;
    bool m_isNested = false;
  };

} /* namespace UNDO */
