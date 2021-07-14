#include "UpdateDocumentMaster.h"
#include "libundo/undo/ContinuousTransaction.h"

UpdateDocumentMaster::UpdateDocumentMaster()
    : UpdateDocumentContributor(nullptr)
{
}

UpdateDocumentMaster::~UpdateDocumentMaster() = default;

UpdateDocumentMaster::tUpdateID UpdateDocumentMaster::onChange(uint64_t flags)
{
  m_flags |= flags;
  incUpdateID();
  return getUpdateIDOfLastChange();
}

uint64_t UpdateDocumentMaster::getAndResetFlags()
{
  return std::exchange(m_flags, 0);
}
