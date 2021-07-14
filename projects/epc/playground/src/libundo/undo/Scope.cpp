#include <memory>

#include "Scope.h"
#include "Transaction.h"
#include "ContinuousTransaction.h"
#include "Algorithm.h"
#include "TrashTransaction.h"
#include <nltools/Assert.h>
#include <iostream>
#include "device-settings/DebugLevel.h"
#include <xml/Writer.h>
#include "TransactionLog.h"

namespace UNDO
{

  Scope::Scope(UpdateDocumentContributor *parent)
      : ContentSection(parent)
      , m_undoActions(*this)
      , m_transactionLog(std::make_unique<TransactionLog>())
  {
    reset();
  }

  Scope::~Scope()
  {
    m_transactionLog.reset();
  }

  UpdateDocumentContributor::tUpdateID Scope::onChange(uint64_t flags)
  {
    auto ret = ContentSection::onChange(flags);
    m_transactionLog->addEntryIfChanged(m_undoPosition);
    return ret;
  }

  Transaction *Scope::getRootTransaction() const
  {
    return m_root.get();
  }

  Transaction *Scope::getUndoTransaction() const
  {
    return m_undoPosition;
  }

  Transaction *Scope::getRedoTransaction() const
  {
    return m_redoPosition;
  }

  size_t Scope::getDepth() const
  {
    return (m_undoPosition) ? m_undoPosition->getDepth() + 1 : 1;
  }

  void Scope::reset()
  {
    m_root.reset();
    m_undoPosition = nullptr;
    m_redoPosition = nullptr;
    m_root = std::make_unique<Transaction>(*this, "Root", 0);
    m_root->close();

    m_undoPosition = m_root.get();
    m_transactionLog = std::make_unique<TransactionLog>();

    onChange();
  }

  void Scope::rebase(Transaction *newRoot)
  {
    if(auto parent = newRoot->getPredecessor())
    {
      m_root = parent->exhaust(newRoot);

      if(m_undoPosition == parent)
        m_undoPosition = nullptr;

      onChange();
    }
  }

  Scope::tTransactionScopePtr Scope::startTransaction(const Glib::ustring &name)
  {
    nltools_assertAlways(m_undoPosition == nullptr || m_undoPosition->isClosed());

    auto transaction = std::make_unique<Transaction>(*this, name, getDepth());
    auto ret = std::make_unique<TransactionCreationScope>(transaction.get());
    addTransaction(std::move(transaction));
    return ret;
  }

  Scope::tTransactionScopePtr Scope::startTrashTransaction()
  {
    return std::make_unique<TransactionCreationScope>(new TrashTransaction(), true);
  }

  Scope::tTransactionScopePtr Scope::startTestTransaction()
  {
    return startTransaction("test");
  }

  Scope::tTransactionScopePtr Scope::startCuckooTransaction()
  {
    g_assert(m_undoPosition->isClosed());

    if(m_cuckooTransaction)
    {
      m_cuckooTransaction->reopen();
      return std::make_unique<TransactionCreationScope>(m_cuckooTransaction.get());
    }

    m_cuckooTransaction = std::make_unique<Transaction>(*this, "Cuckoo", getDepth());
    return std::make_unique<TransactionCreationScope>(m_cuckooTransaction.get());
  }

  void Scope::resetCukooTransaction()
  {
    m_cuckooTransaction.reset();
  }

  Scope::tTransactionScopePtr Scope::startContinuousTransaction(void *id, std::chrono::milliseconds timeout,
                                                                const Glib::ustring &name)
  {
    nltools_assertAlways(m_undoPosition == nullptr || m_undoPosition->isClosed());

    auto transaction = std::make_unique<ContinuousTransaction>(*this, id, name, getDepth());

    if(!canRedo())
    {
      if(auto last = dynamic_cast<ContinuousTransaction *>(getUndoTransaction()))
      {
        if(last->isContinuing() && last->getID() == id && last->getAge() <= timeout)
        {
          auto ret = std::make_unique<TransactionCreationScope>(transaction.get());
          last->setClosingCommand(std::move(transaction));
          return ret;
        }
      }
    }
    else if(auto redo = dynamic_cast<ContinuousTransaction *>(getRedoTransaction()))
    {
      if(redo->getNumSuccessors() == 0)
      {
        if(redo->getID() == id && redo->getAge() <= timeout)
        {
          eraseBranch(m_redoPosition);
        }
      }
    }

    auto ret = std::make_unique<TransactionCreationScope>(transaction.get());
    addTransaction(std::move(transaction));
    return ret;
  }

  void Scope::addTransaction(tTransactionPtr successor)
  {
    DebugLevel::info("UNDO::Scope::addTransaction");
    auto raw = successor.get();

    if(m_undoPosition)
      m_undoPosition->addSuccessor(std::move(successor));
    else
      throw std::runtime_error("unexpected");

    raw->setPredecessor(m_undoPosition);
    m_undoPosition = raw;
    m_redoPosition = nullptr;

    if(m_cuckooTransaction)
      raw->addCommand(std::move(m_cuckooTransaction));

    onAddTransaction(raw);
    m_undoPosition->doAction();
    onTransactionAdded();
  }

  void Scope::onTransactionAdded()
  {
    onChange();
  }

  void Scope::onAddTransaction(Transaction *transaction)
  {
  }

  void Scope::onTransactionUndoStart()
  {
    nltools_assertAlways(m_undoPosition != nullptr);
    m_redoPosition = m_undoPosition;
    m_undoPosition = m_undoPosition->getPredecessor();

    if(m_undoPosition)
      m_undoPosition->setDefaultRedoRoute(m_redoPosition);
  }

  void Scope::onTransactionRedone(const Transaction *transaction)
  {
    nltools_assertAlways(m_redoPosition != nullptr);
    size_t numWays = m_undoPosition->getNumSuccessors();

    for(size_t i = 0; i < numWays; ++i)
    {
      if(m_undoPosition->getSuccessor(i) == transaction)
      {
        m_undoPosition = m_undoPosition->getSuccessor(i);
        m_redoPosition = m_undoPosition->getDefaultRedoRoute();
        return;
      }
    }
  }

  bool Scope::canUndo() const
  {
    return m_undoPosition != m_root.get();
  }

  bool Scope::canRedo() const
  {
    return m_redoPosition != nullptr;
  }

  void Scope::undo()
  {
    if(canUndo())
    {
      if(auto undo = getUndoTransaction())
      {
        if(m_cuckooTransaction)
        {
          undo->reopen();
          undo->addCommand(std::move(m_cuckooTransaction));
          undo->close();
        }
        undo->undoAction();
        onChange();
      }
    }
  }

  void Scope::undoAndHushUp()
  {
    if(canUndo())
    {
      auto oldUndo = m_undoPosition;
      undo();
      eraseBranch(oldUndo);
      m_redoPosition = nullptr;
    }
  }

  void Scope::redo(int way)
  {
    if(auto redo = getRedoTransaction())
    {
      if(way == -1)
      {
        m_cuckooTransaction.reset();
        redo->redoAction();
        onChange();
      }
      else
      {
        UNDO::Transaction *predecessor = redo->getPredecessor();

        if(predecessor->getNumSuccessors() > (size_t) way)
        {
          m_cuckooTransaction.reset();
          auto p = predecessor->getSuccessor(way);
          p->redoAction();
          onChange();
        }
      }
    }
  }

  void Scope::undoJump(const Glib::ustring &target)
  {
    if(auto foundTransaction = Algorithm::find(getRootTransaction(), target))
    {
      undoJump(foundTransaction);
    }
  }

  void Scope::undoJump(Transaction *target)
  {
    Algorithm::traverse(getUndoTransaction(), target);
    onChange();
  }

  void Scope::eraseBranch(const Glib::ustring &id)
  {
    if(auto foundTransaction = Algorithm::find(getRootTransaction(), id))
    {
      eraseBranch(foundTransaction);
    }
  }

  void Scope::eraseBranch(Transaction *branch)
  {
    if(auto parent = branch->getPredecessor())
    {
      parent->eraseSuccessor(branch);

      if(m_redoPosition == branch)
        m_redoPosition = nullptr;
    }
  }

  Glib::ustring Scope::getPrefix() const
  {
    return m_undoActions.getBasePath().substr(1);
  }

  void Scope::handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path)
  {
    m_undoActions.handleRequest(path, request);
  }

  void Scope::writeDocument(Writer &writer, tUpdateID knownRevision) const
  {
    if(knownRevision < getUpdateIDOfLastChange())
    {
      writer.writeTag("undo", [&]() {
        if(getUndoTransaction() && canUndo())
          writer.writeTextElement("undo", StringTools::buildString(getUndoTransaction()));
        else
          writer.writeTextElement("undo", "0");

        writer.writeTextElement("redo", StringTools::buildString(getRedoTransaction()));

        getRootTransaction()->traverse([&](const Transaction *p) mutable { p->writeDocument(writer, knownRevision); });
      });
    }
  }

  void Scope::onTransactionDestroyed(const Transaction *p)
  {
    if(m_transactionLog)
      m_transactionLog->removeTransaction(p);
  }

  const Transaction *Scope::findTransactionAt(std::chrono::system_clock::time_point timestamp) const
  {
    return m_transactionLog->findRecentTransactionAt(timestamp);
  }

} /* namespace UNDO */
