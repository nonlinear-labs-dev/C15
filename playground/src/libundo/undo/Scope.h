#pragma once

#include "TransactionCreationScope.h"
#include "SwapData.h"
#include "StringTools.h"
#include "http/ContentSection.h"
#include "http/UndoActions.h"
#include <chrono>

namespace UNDO
{
  using chrono::steady_clock;

  class Scope : public ContentSection
  {
   public:
    typedef TransactionCreationScope::tTransactionPtr tTransactionPtr;
    typedef TransactionCreationScope::tTransactionScopePtr tTransactionScopePtr;

    constexpr static steady_clock::duration getStandardContinuousTransactionTimeout()
    {
      return chrono::seconds(2);
    }

    Scope(UpdateDocumentContributor *parent);
    virtual ~Scope();

    void reset();

    template <typename... tArgs>
    tTransactionScopePtr startTransaction(const Glib::ustring &format, const tArgs &... args)
    {
      return startTransaction(StringTools::formatString(format, args...));
    }

    template <typename... tArgs>
    tTransactionScopePtr startContinuousTransaction(void *id, steady_clock::duration timeout,
                                                    const Glib::ustring &format, const tArgs &... args)
    {
      return startContinuousTransaction(id, timeout, StringTools::formatString(format, args...));
    }

    template <typename... tArgs>
    tTransactionScopePtr startContinuousTransaction(void *id, const Glib::ustring &format, const tArgs &... args)
    {
      return startContinuousTransaction(id, getStandardContinuousTransactionTimeout(),
                                        StringTools::formatString(format, args...));
    }

    tTransactionScopePtr startContinuousTransaction(void *id, steady_clock::duration timeout,
                                                    const Glib::ustring &name);

    tTransactionScopePtr startTransaction(const Glib::ustring &name);
    tTransactionScopePtr startTrashTransaction();

    tTransactionScopePtr startCuckooTransaction();
    void resetCukooTransaction();

    const tTransactionPtr getUndoTransaction() const;
    const tTransactionPtr getRedoTransaction() const;
    const tTransactionPtr getRootTransaction() const;

    void rebase(tTransactionPtr newRoot);

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void undoAndHushUp();
    void redo(int way = -1);

    void undoJump(const Glib::ustring &target);
    void undoJump(shared_ptr<Transaction> target);

    void eraseBranch(const Glib::ustring &id);
    void eraseBranch(shared_ptr<Transaction> branch);

    Glib::ustring getPrefix() const override;
    void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
    void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

   protected:
    virtual void onTransactionAdded();
    virtual void onAddTransaction(tTransactionPtr transaction);

   private:
    Scope(const Scope &other);
    void operator=(const Scope &other);

    size_t getDepth() const;
    void addTransaction(tTransactionPtr cmd);

    friend class Transaction;
    void onTransactionUndoStart();
    void onTransactionRedone(const Transaction *transaction);

    UndoActions m_undoActions;

    tTransactionPtr m_root;
    tTransactionPtr m_undoPosition;
    tTransactionPtr m_redoPosition;
    tTransactionPtr m_cuckooTransaction;
  };

} /* namespace UNDO */
