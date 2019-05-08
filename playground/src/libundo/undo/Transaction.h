#pragma once

#include "ActionCommand.h"
#include <http/UpdateDocumentContributor.h>
#include <memory>
#include <list>

namespace UNDO
{
  class Scope;

  class Transaction : public Command, public UpdateDocumentContributor
  {
   public:
    typedef std::shared_ptr<Transaction> tTransactionPtr;

    Transaction(Scope &scope, const Glib::ustring &name, size_t depth);
    virtual ~Transaction();

    static int getAndResetNumTransactions();

    Glib::ustring getName() const;
    void setName(const Glib::ustring &name);

    void addSimpleCommand(ActionCommand::tAction doAndRedo, ActionCommand::tAction undo);
    void addSimpleCommand(ActionCommand::tAction doRedoUndo);

    virtual void addCommand(tCommandPtr cmd);
    void close();
    void reopen();
    bool isClosed() const;
    void rollBack();

    int countPredecessors() const;
    int countSuccessorsOnDefaultRoute() const;

    bool hasSuccessors() const;
    size_t getNumSuccessors() const;
    const tTransactionPtr getPredecessor() const;
    void setPredecessor(tTransactionPtr predecessor);
    const tTransactionPtr getSuccessor(size_t num) const;
    void addSuccessor(tTransactionPtr successor);
    void eraseSuccessor(tTransactionPtr successor);
    void eraseSuccessor(const Transaction *successor);

    size_t getDepth() const;

    void redoUntil(tTransactionPtr target);
    void undoUntil(tTransactionPtr target);

    void addChildren(std::list<tTransactionPtr> &list) const;

    void setDefaultRedoRoute(tTransactionPtr route);
    tTransactionPtr getDefaultRedoRoute() const;

    bool isDefaultRouteLeft() const;
    bool isDefaultRouteRight() const;

    void addPostfixCommand(ActionCommand::tAction doRedoUndo);

    virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

    void recurse(function<void(const Transaction *)> cb) const;

   protected:
    virtual void implDoAction() const override;
    virtual void implUndoAction() const override;
    virtual void implRedoAction() const override;

    virtual void onImplUndoActionStart() const;
    virtual void onImplRedoActionFinished() const;

   private:
    Glib::ustring getSuccessorsString() const;
    int getOrder(const Transaction *a, const Transaction *b) const;
    int getDirectionToDefaultRoute() const;

    typedef std::list<tCommandPtr> tCommandList;
    typedef std::vector<tTransactionPtr> tSuccessors;

    Scope &m_scope;
    Glib::ustring m_name;
    tCommandList m_commands;
    bool m_isClosed;

    weak_ptr<Transaction> m_predecessor;
    tSuccessors m_successors;
    tTransactionPtr m_defaultRedoRoute;
    tCommandList m_postfixCommands;

    size_t m_depth;
  };

} /* namespace UNDO */
