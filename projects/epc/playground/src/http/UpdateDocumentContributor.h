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

  explicit UpdateDocumentContributor(UpdateDocumentContributor *parent);
  virtual ~UpdateDocumentContributor();

  UpdateDocumentContributor(const UpdateDocumentContributor &other) = delete;
  UpdateDocumentContributor &operator=(const UpdateDocumentContributor &) = delete;

  enum ChangeFlags : uint64_t
  {
    Generic = 1 << 0,
    LockState = 1 << 1,
    DontTrustOracle = 1 << 2
  };

  virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic);

  [[nodiscard]] bool didChangeSince(tUpdateID clientsUpdateID) const;
  [[nodiscard]] tUpdateID getUpdateIDOfLastChange() const;

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const = 0;

  [[nodiscard]] UpdateDocumentContributor *getParent() const;
  UpdateDocumentMaster *getRoot();
  [[nodiscard]] const UpdateDocumentMaster *getRoot() const;

  virtual UNDO::Scope &getUndoScope();
  [[nodiscard]] virtual const UNDO::Scope &getUndoScope() const;

  void orphan();

 protected:
  void setUpdateID(tUpdateID id);
  void incUpdateID();

  void adopt(UpdateDocumentContributor *parent);

 private:
  using super::getNext;
  using super::getPrev;

  UpdateDocumentContributor *m_parent = nullptr;
  IntrusiveList<UpdateDocumentContributor *> m_children;

  tUpdateID m_updateIDOnLastChange;
};
