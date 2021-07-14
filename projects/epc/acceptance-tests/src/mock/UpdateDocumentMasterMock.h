#pragma once
#include <http/UpdateDocumentMaster.h>
#include <libundo/undo/Scope.h>

class UpdateDocumentMasterMock : public UpdateDocumentMaster
{
 public:
  UpdateDocumentMasterMock()
      : m_scope(this)
  {
  }

  void writeDocument(Writer &, tUpdateID) const override
  {
  }

  UNDO::Scope &getUndoScope() override
  {
    return m_scope;
  }

  const UNDO::Scope &getUndoScope() const override
  {
    return m_scope;
  }

 private:
  UNDO::Scope m_scope;
};