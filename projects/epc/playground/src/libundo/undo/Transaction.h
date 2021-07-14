#pragma once

#include "ActionCommand.h"
#include <libundo/undo/SwapData.h>
#include <http/UpdateDocumentContributor.h>
#include <memory>
#include <list>
#include <vector>
#include <chrono>

class Writer;

namespace UNDO
{
  class Scope;

  class Transaction : public Command, public UpdateDocumentContributor
  {
   public:
    Transaction(Scope &scope, const Glib::ustring &name, size_t depth);
    ~Transaction() override;

    static int getAndResetNumTransactions();

    Glib::ustring getName() const;
    void setName(const Glib::ustring &name);

    virtual void addSimpleCommand(ActionCommand::tAction doAndRedo, ActionCommand::tAction undo);
    virtual void addSimpleCommand(ActionCommand::tAction doRedoUndo);
    virtual void addCommand(tCommandPtr cmd);

    void close();
    void reopen();
    bool isClosed() const;
    void rollBack();

    int countPredecessors() const;
    int countSuccessorsOnDefaultRoute() const;

    bool hasSuccessors() const;
    size_t getNumSuccessors() const;
    Transaction *getPredecessor() const;
    void setPredecessor(Transaction *predecessor);
    Transaction *getSuccessor(size_t num) const;
    void addSuccessor(std::unique_ptr<Transaction> successor);
    void eraseSuccessor(const Transaction *successor);

    std::unique_ptr<Transaction> exhaust(const Transaction *a);

    size_t getDepth() const;

    void redoUntil(Transaction *target);
    void undoUntil(Transaction *target);

    void addChildren(std::list<Transaction *> &list) const;

    void setDefaultRedoRoute(Transaction *route);
    Transaction *getDefaultRedoRoute() const;

    bool isDefaultRouteLeft() const;
    bool isDefaultRouteRight() const;

    void addPostfixCommand(ActionCommand::tAction doRedoUndo);

    void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

    long traverseTree() const;
    void traverse(std::function<void(const Transaction *)> cb) const;

    template <typename T> void addUndoSwap(UpdateDocumentContributor *c, T &target, const T &newValue)
    {
      if(target != newValue)
      {
        addSimpleCommand([c, &target, swap = UNDO::createSwapData(std::move(newValue))](auto) {
          swap->swapWith(target);
          c->onChange();
        });
      }
    }

    template <typename T> void addUndoSwap(UpdateDocumentContributor *c, T &target, T &&newValue)
    {
      if(target != newValue)
      {
        addSimpleCommand([c, &target, swap = UNDO::createSwapData(std::forward<T>(newValue))](auto) {
          swap->swapWith(target);
          c->onChange();
        });
      }
    }

    template <typename T> void addUndoSwap(T &target, const T &newValue)
    {
      if(target != newValue)
      {
        addSimpleCommand([&target, swap = UNDO::createSwapData(std::move(newValue))](auto) { swap->swapWith(target); });
      }
    }

    template <typename T> void addUndoSwap(T &target, T &&newValue)
    {
      if(target != newValue)
      {
        addSimpleCommand(
            [&target, swap = UNDO::createSwapData(std::forward<T>(newValue))](auto) { swap->swapWith(target); });
      }
    }

   protected:
    void implDoAction() const override;
    void implUndoAction() const override;
    void implRedoAction() const override;

    virtual void onImplUndoActionStart() const;
    virtual void onImplRedoActionFinished() const;

   private:
    Glib::ustring getSuccessorsString() const;
    int getOrder(const Transaction *a, const Transaction *b) const;
    int getDirectionToDefaultRoute() const;

    typedef std::list<tCommandPtr> tCommandList;
    typedef std::vector<std::unique_ptr<Transaction>> tSuccessors;

    Scope &m_scope;
    Glib::ustring m_name;
    tCommandList m_commands;
    bool m_isClosed = false;

    Transaction *m_predecessor = nullptr;
    Transaction *m_defaultRedoRoute = nullptr;

    tSuccessors m_successors;
    tCommandList m_postfixCommands;

    size_t m_depth = 0;
  };

} /* namespace UNDO */
