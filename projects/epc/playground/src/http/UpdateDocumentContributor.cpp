#include "UpdateDocumentContributor.h"
#include "UpdateDocumentMaster.h"
#include "libundo/undo/Transaction.h"

UpdateDocumentContributor::UpdateDocumentContributor(UpdateDocumentContributor *parent)
    : m_parent(parent)
    , m_updateIDOnLastChange(1)
{
  if(m_parent)
  {
    m_updateIDOnLastChange = m_parent->getUpdateIDOfLastChange() + 1;
    m_parent->m_children.prepend(this);
  }
}

UpdateDocumentContributor::~UpdateDocumentContributor()
{
  for(UpdateDocumentContributor *c : m_children)
  {
    c->orphan();
  }

  if(m_parent)
  {
    m_parent->m_children.remove(this);
  }
}

void UpdateDocumentContributor::orphan()
{
  m_parent = nullptr;
}

void UpdateDocumentContributor::adopt(UpdateDocumentContributor *parent)
{
  m_parent = parent;
}

UpdateDocumentContributor::tUpdateID UpdateDocumentContributor::onChange(uint64_t flags)
{
  m_updateIDOnLastChange = m_parent->onChange(flags);
  return m_updateIDOnLastChange;
}

bool UpdateDocumentContributor::didChangeSince(tUpdateID clientsUpdateID) const
{
  return m_updateIDOnLastChange > clientsUpdateID || clientsUpdateID == 0;
}

UpdateDocumentContributor::tUpdateID UpdateDocumentContributor::getUpdateIDOfLastChange() const
{
  return m_updateIDOnLastChange;
}

void UpdateDocumentContributor::setUpdateID(tUpdateID id)
{
  m_updateIDOnLastChange = id;
}

void UpdateDocumentContributor::incUpdateID()
{
  m_updateIDOnLastChange++;
}

UpdateDocumentContributor *UpdateDocumentContributor::getParent() const
{
  return m_parent;
}

UpdateDocumentMaster *UpdateDocumentContributor::getRoot()
{
  if(m_parent)
    return m_parent->getRoot();

  return static_cast<UpdateDocumentMaster *>(this);
}

const UpdateDocumentMaster *UpdateDocumentContributor::getRoot() const
{
  if(m_parent)
    return m_parent->getRoot();

  return static_cast<const UpdateDocumentMaster *>(this);
}

UNDO::Scope &UpdateDocumentContributor::getUndoScope()
{
  return getParent()->getUndoScope();
}

const UNDO::Scope &UpdateDocumentContributor::getUndoScope() const
{
  return getParent()->getUndoScope();
}
