#include "ContinuousTransaction.h"
#include <memory>

namespace UNDO
{

  ContinuousTransaction::ContinuousTransaction(Scope &scope, void *id, const Glib::ustring &name, size_t depth)
      : Transaction(scope, name, depth)
      , m_id(id)
  {
    m_creationTimestamp = steady_clock::now();
  }

  ContinuousTransaction::~ContinuousTransaction()
  {
  }

  std::chrono::steady_clock::duration ContinuousTransaction::getAge() const
  {
    return steady_clock::now() - m_creationTimestamp;
  }

  void *ContinuousTransaction::getID() const
  {
    return m_id;
  }

  void ContinuousTransaction::setClosingCommand(std::unique_ptr<ContinuousTransaction> command)
  {
    m_closingCommand = std::move(command);
    m_closingCommand->setIsNested();
    m_closingCommand->doAction();
    setName(m_closingCommand->getName());
    onChange();
  }

  ContinuousTransaction *ContinuousTransaction::getClosingCommand() const
  {
    return m_closingCommand.get();
  }

  void ContinuousTransaction::stopContinuation()
  {
    m_continuing = false;
  }

  bool ContinuousTransaction::isContinuing() const
  {
    return m_continuing;
  }

  void ContinuousTransaction::implUndoAction() const
  {
    if(m_closingCommand)
      m_closingCommand->undoAction();

    super::implUndoAction();
  }

  void ContinuousTransaction::implRedoAction() const
  {
    super::implRedoAction();

    if(m_closingCommand)
      m_closingCommand->redoAction();
  }

  void ContinuousTransaction::setIsNested()
  {
    m_isNested = true;
  }

  void ContinuousTransaction::onImplUndoActionStart() const
  {
    if(!m_isNested)
      super::onImplUndoActionStart();
  }

  void ContinuousTransaction::onImplRedoActionFinished() const
  {
    if(!m_isNested)
      super::onImplRedoActionFinished();
  }

} /* namespace UNDO */
