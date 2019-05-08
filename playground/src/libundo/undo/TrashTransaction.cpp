#include "TrashTransaction.h"
#include "Scope.h"
#include <http/UpdateDocumentMaster.h>

namespace UNDO
{
  class TrashUpdateDocMaster : public UpdateDocumentMaster
  {
   public:
    TrashUpdateDocMaster()
    {
    }

    virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override
    {
    }
  };

  static TrashUpdateDocMaster trashDocMaster;

  class TrashScope : public Scope
  {
   public:
    TrashScope()
        : Scope(&trashDocMaster)
    {
    }

    tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override
    {
      return 0;
    }
  };

  static TrashScope trashScope;

  TrashTransaction::TrashTransaction()
      : Transaction(trashScope, "trash", 0)
  {
  }

  TrashTransaction::~TrashTransaction()
  {
  }

  void TrashTransaction::addCommand(Command::tCommandPtr cmd)
  {
    cmd->setParentTransaction(this);
    cmd->doAction();
  }
}
