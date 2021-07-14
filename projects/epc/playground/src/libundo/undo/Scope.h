#pragma once

#include "SwapData.h"
#include "StringTools.h"
#include "http/ContentSection.h"
#include "http/UndoActions.h"
#include "TransactionCreationScope.h"
#include <chrono>

namespace UNDO
{
  class TransactionCreationScope;
  class Transaction;

  class TransactionLog;

  class Scope : public ContentSection
  {
   public:
    using tTransactionScopePtr = std::unique_ptr<TransactionCreationScope>;

    constexpr static std::chrono::milliseconds getStandardContinuousTransactionTimeout()
    {
      return std::chrono::seconds(2);
    }

    explicit Scope(UpdateDocumentContributor *parent);
    ~Scope() override;

    tUpdateID onChange(uint64_t flags = ChangeFlags::Generic) override;

    void reset();

    template <typename... tArgs>
    tTransactionScopePtr startTransaction(const Glib::ustring &format, const tArgs &... args)
    {
      return startTransaction(StringTools::formatString(format, args...));
    }

    template <typename... tArgs>

    tTransactionScopePtr startContinuousTransaction(void *id, std::chrono::milliseconds timeout,
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

    tTransactionScopePtr startContinuousTransaction(void *id, std::chrono::milliseconds timeout,
                                                    const Glib::ustring &name);

    tTransactionScopePtr startTransaction(const Glib::ustring &name);
    static tTransactionScopePtr startTrashTransaction();
    tTransactionScopePtr startTestTransaction();

    tTransactionScopePtr startCuckooTransaction();
    void resetCukooTransaction();

    [[nodiscard]] Transaction *getUndoTransaction() const;
    [[nodiscard]] Transaction *getRedoTransaction() const;
    [[nodiscard]] Transaction *getRootTransaction() const;

    void rebase(Transaction *newRoot);

    [[nodiscard]] bool canUndo() const;
    [[nodiscard]] bool canRedo() const;

    void undo();
    void undoAndHushUp();
    void redo(int way = -1);

    void undoJump(const Glib::ustring &target);
    void undoJump(Transaction *target);

    void eraseBranch(const Glib::ustring &id);
    void eraseBranch(Transaction *branch);

    [[nodiscard]] Glib::ustring getPrefix() const override;
    void handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
    void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

    [[nodiscard]] const Transaction *findTransactionAt(std::chrono::system_clock::time_point timestamp) const;

    void onTransactionDestroyed(const Transaction *p);

   protected:
    virtual void onTransactionAdded();
    virtual void onAddTransaction(Transaction *transaction);

   private:
    using tTransactionPtr = std::unique_ptr<Transaction>;

    Scope(const Scope &other);
    void operator=(const Scope &other);

    [[nodiscard]] size_t getDepth() const;
    void addTransaction(tTransactionPtr cmd);

    friend class Transaction;
    void onTransactionUndoStart();
    void onTransactionRedone(const Transaction *transaction);

    std::unique_ptr<TransactionLog> m_transactionLog;

    UndoActions m_undoActions;

    tTransactionPtr m_root;
    tTransactionPtr m_cuckooTransaction;

    Transaction *m_undoPosition = nullptr;
    Transaction *m_redoPosition = nullptr;
  };

} /* namespace UNDO */
