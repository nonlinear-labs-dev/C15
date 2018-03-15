#include "UpdateDocumentMaster.h"
#include "libundo/undo/ContinuousTransaction.h"

UpdateDocumentMaster::UpdateDocumentMaster() :
  UpdateDocumentContributor (NULL)
{
}

UpdateDocumentMaster::~UpdateDocumentMaster()
{
}

UpdateDocumentMaster::tUpdateID UpdateDocumentMaster::onChange (uint64_t flags)
{
  incUpdateID();
  return getUpdateIDOfLastChange();
}

