#pragma once

#include <memory>

namespace UNDO
{
  class Transaction;

  class Command
  {
   public:
    typedef std::shared_ptr<Command> tCommandPtr;

    Command();
    virtual ~Command();

    void setParentTransaction(Transaction *parent);
    bool hasParentTransaction() const;

    void doAction() const;
    void undoAction() const;
    void redoAction() const;

    enum State
    {
      INITIAL,
      DOING,
      DONE,
      UNDOING,
      UNDONE,
      REDOING,
      REDONE
    };

    State getState() const;

   protected:
    virtual void implDoAction() const = 0;
    virtual void implUndoAction() const = 0;
    virtual void implRedoAction() const = 0;

   private:
    Transaction *m_parent = nullptr;
    mutable State m_state;
  };

} /* namespace UNDO */
