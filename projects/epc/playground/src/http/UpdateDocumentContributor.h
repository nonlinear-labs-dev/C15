#pragma once

#include "playground.h"
#include "tools/IntrusiveList.h"

namespace UNDO
{
  class Scope;
}

class Writer;
class UpdateDocumentMaster;

class UpdateDocumentContributor : public IntrusiveListItem<UpdateDocumentContributor *>
{
 private:
  using super = IntrusiveListItem<UpdateDocumentContributor *>;

 public:
  typedef int tUpdateID;

  UpdateDocumentContributor(UpdateDocumentContributor *parent);
  virtual ~UpdateDocumentContributor();

  UpdateDocumentContributor(const UpdateDocumentContributor &other) = delete;
  UpdateDocumentContributor &operator=(const UpdateDocumentContributor &) = delete;

  enum ChangeFlags : uint64_t
  {
    Generic = 1 << 0,
    LockState = 1 << 1,
  };

  virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic);

  bool didChangeSince(tUpdateID clientsUpdateID) const;
  tUpdateID getUpdateIDOfLastChange() const;

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const = 0;

  UpdateDocumentContributor *getParent() const;
  UpdateDocumentMaster *getRoot();
  const UpdateDocumentMaster *getRoot() const;

  virtual UNDO::Scope &getUndoScope();
  virtual const UNDO::Scope &getUndoScope() const;

  void propagateChangeDownstream();
  void orphan();
  void adopt(UpdateDocumentContributor *parent);

 protected:
  void setUpdateID(tUpdateID id);
  void incUpdateID();

 private:
  using super::getNext;
  using super::getPrev;

  UpdateDocumentContributor *m_parent = nullptr;
  IntrusiveList<UpdateDocumentContributor *> m_children;

  tUpdateID m_updateIDOnLastChange;
};
