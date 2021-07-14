#pragma once

#include <http/UpdateDocumentContributor.h>
#include <map>

namespace UNDO
{
  class Transaction;
}

class AttributesOwner : public UpdateDocumentContributor
{
 public:
  typedef std::map<std::string, std::string> tAttributes;

  explicit AttributesOwner(UpdateDocumentContributor *parent);
  AttributesOwner(UpdateDocumentContributor *parent, const AttributesOwner *other);
  ~AttributesOwner() override;

  virtual void setAttribute(UNDO::Transaction *transaction, const std::string &key, const Glib::ustring &value);
  virtual void copyFrom(UNDO::Transaction *transaction, const AttributesOwner *other);
  virtual void clear(UNDO::Transaction *transaction);

  [[nodiscard]] Glib::ustring getAttribute(const Glib::ustring &key, const Glib::ustring &def) const;
  [[nodiscard]] const tAttributes &getAttributes() const;
  [[nodiscard]] size_t getHash() const;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void writeDiff(Writer &writer, const AttributesOwner *other) const;

 private:
  tAttributes m_attributes;

  friend class AttributesOwnerSerializer;
};
