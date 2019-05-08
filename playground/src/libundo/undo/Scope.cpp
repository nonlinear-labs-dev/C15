#include "Scope.h"
#include "Transaction.h"
#include "ContinuousTransaction.h"
#include "Algorithm.h"
#include "TrashTransaction.h"
#include <assert.h>
#include <iostream>
#include "device-settings/DebugLevel.h"
#include <xml/Writer.h>

namespace UNDO
{

  Scope::Scope(UpdateDocumentContributor *parent)
      : ContentSection(parent)
      , m_undoActions(*this)
  {
    tTransactionScopePtr rootScope = startTransaction("Root");
    m_root = rootScope->getTransaction();
  }

  Scope::~Scope()
  {
    DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
  }

  const Scope::tTransactionPtr Scope::getRootTransaction() const
  {
    return m_root;
  }

  const Scope::tTransactionPtr Scope::getUndoTransaction() const
  {
    return m_undoPosition;
  }

  const Scope::tTransactionPtr Scope::getRedoTransaction() const
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
    m_undoPosition.reset();
    m_redoPosition.reset();

    tTransactionScopePtr rootScope = startTransaction("Big Bang");
    m_root = rootScope->getTransaction();
  }

  void Scope::rebase(Scope::tTransactionPtr newRoot)
  {
    m_root = newRoot;

    if(m_undoPosition == m_root->getPredecessor())
      m_undoPosition.reset();

    m_root->setPredecessor(nullptr);
    onChange();
  }

  Scope::tTransactionScopePtr Scope::startTransaction(const Glib::ustring &name)
  {
    assert(m_undoPosition == NULL || m_undoPosition->isClosed());

    std::shared_ptr<Transaction> transaction(new Transaction(*this, name, getDepth()));
    addTransaction(transaction);
    return Scope::tTransactionScopePtr(new TransactionCreationScope(transaction));
  }

  Scope::tTransactionScopePtr Scope::startTrashTransaction()
  {
    std::shared_ptr<Transaction> transaction(new TrashTransaction());
    return Scope::tTransactionScopePtr(new TransactionCreationScope(transaction));
  }

  Scope::tTransactionScopePtr Scope::startCuckooTransaction()
  {
    g_assert(m_undoPosition->isClosed());

    if(m_cuckooTransaction)
    {
      m_cuckooTransaction->reopen();
      return Scope::tTransactionScopePtr(new TransactionCreationScope(m_cuckooTransaction));
    }

    m_cuckooTransaction.reset(new Transaction(*this, "Cuckoo", getDepth()));
    return Scope::tTransactionScopePtr(new TransactionCreationScope(m_cuckooTransaction));
  }

  void Scope::resetCukooTransaction()
  {
    m_cuckooTransaction.reset();
  }

  Scope::tTransactionScopePtr Scope::startContinuousTransaction(void *id, steady_clock::duration timeout,
                                                                const Glib::ustring &name)
  {
    assert(m_undoPosition == NULL || m_undoPosition->isClosed());

    std::shared_ptr<ContinuousTransaction> transaction(new ContinuousTransaction(*this, id, name, getDepth()));

    if(!canRedo())
    {
      if(shared_ptr<ContinuousTransaction> last = dynamic_pointer_cast<ContinuousTransaction>(getUndoTransaction()))
      {
        if(last->getID() == id && last->getAge() <= timeout)
        {
          last->setClosingCommand(transaction);
          return Scope::tTransactionScopePtr(new TransactionCreationScope(transaction));
        }
      }
    }
    else if(shared_ptr<ContinuousTransaction> redo = dynamic_pointer_cast<ContinuousTransaction>(getRedoTransaction()))
    {
      if(redo->getNumSuccessors() == 0)
      {
        if(redo->getID() == id && redo->getAge() <= timeout)
        {
          eraseBranch(m_redoPosition);
        }
      }
    }

    addTransaction(transaction);
    return Scope::tTransactionScopePtr(new TransactionCreationScope(transaction));
  }

  void Scope::addTransaction(tTransactionPtr successor)
  {
    DebugLevel::info("UNDO::Scope::addTransaction");

    if(m_undoPosition)
      m_undoPosition->addSuccessor(successor);

    successor->setPredecessor(m_undoPosition);

    m_undoPosition = successor;
    m_redoPosition = NULL;

    if(m_cuckooTransaction)
    {
      successor->addCommand(m_cuckooTransaction);
      m_cuckooTransaction.reset();
    }

    onAddTransaction(successor);

    m_undoPosition->doAction();

    onTransactionAdded();
  }

  void Scope::onTransactionAdded()
  {
  }

  void Scope::onAddTransaction(tTransactionPtr transaction)
  {
  }

  void Scope::onTransactionUndoStart()
  {
    assert(m_undoPosition != NULL);
    m_redoPosition = m_undoPosition;
    m_undoPosition = m_undoPosition->getPredecessor();

    if(m_undoPosition)
      m_undoPosition->setDefaultRedoRoute(m_redoPosition);
  }

  void Scope::onTransactionRedone(const Transaction *transaction)
  {
    assert(m_redoPosition != NULL);
    size_t numWays = m_undoPosition->getNumSuccessors();

    for(size_t i = 0; i < numWays; ++i)
    {
      if(m_undoPosition->getSuccessor(i).get() == transaction)
      {
        m_undoPosition = m_undoPosition->getSuccessor(i);
        m_redoPosition = m_undoPosition->getDefaultRedoRoute();
        return;
      }
    }
  }

  bool Scope::canUndo() const
  {
    return m_undoPosition != m_root;
  }

  bool Scope::canRedo() const
  {
    return m_redoPosition != NULL;
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
          undo->addCommand(m_cuckooTransaction);
          undo->close();

          m_cuckooTransaction.reset();
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
      m_redoPosition.reset();
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
        UNDO::Scope::tTransactionPtr predecessor = redo->getPredecessor();

        if(predecessor->getNumSuccessors() > (size_t) way)
        {
          m_cuckooTransaction.reset();
          predecessor->getSuccessor(way)->redoAction();
          onChange();
        }
      }
    }
  }

  void Scope::undoJump(const Glib::ustring &target)
  {
    if(tTransactionPtr foundTransaction = Algorithm::find(getRootTransaction(), target))
    {
      undoJump(foundTransaction);
    }
  }

  void Scope::undoJump(shared_ptr<Transaction> target)
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

  void Scope::eraseBranch(shared_ptr<Transaction> branch)
  {
    if(auto parent = branch->getPredecessor())
    {
      parent->eraseSuccessor(branch);

      if(m_redoPosition == branch)
        m_redoPosition.reset();
    }
  }

  Glib::ustring Scope::getPrefix() const
  {
    return m_undoActions.getBasePath().substr(1);
  }

  void Scope::handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path)
  {
    m_undoActions.handleRequest(path, request);
  }

  void Scope::writeDocument(Writer &writer, tUpdateID knownRevision) const
  {
    if(knownRevision < getUpdateIDOfLastChange())
    {
      writer.writeTag("undo", [&]() {
        if(getUndoTransaction() && canUndo())
          writer.writeTextElement("undo", StringTools::buildString(getUndoTransaction().get()));
        else
          writer.writeTextElement("undo", "0");

        writer.writeTextElement("redo", StringTools::buildString(getRedoTransaction().get()));

        getRootTransaction()->recurse([&](const Transaction *p) mutable { p->writeDocument(writer, knownRevision); });
      });
    }
  }

} /* namespace UNDO */
