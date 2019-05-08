#pragma once

#include <glibmm/ustring.h>
#include <libundo/undo/Scope.h>
#include <xml/Writer.h>
#include <map>

class AttributesOwner
{
 public:
  typedef std::map<Glib::ustring, Glib::ustring> tAttributes;

  AttributesOwner();
  virtual ~AttributesOwner();

  virtual UpdateDocumentContributor::tUpdateID onChange(uint64_t flags
                                                        = UpdateDocumentContributor::ChangeFlags::Generic)
      = 0;

  void setAttribute(const Glib::ustring &key, const Glib::ustring &value);
  void undoableSetAttribute(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &key,
                            const Glib::ustring &value);
  void undoableClearAttributes(UNDO::Scope::tTransactionPtr transaction);

  void copyFrom(UNDO::Scope::tTransactionPtr transaction, AttributesOwner *other);
  size_t getHash() const;
  bool doesAnyAttributeMatch(const Glib::ustring &part) const;

  Glib::ustring getAttribute(const Glib::ustring &key, const Glib::ustring &def) const;
  const tAttributes &getAttributes() const;
  void writeAttributes(Writer &writer) const;

  static void registerTests();

 private:
  static bool isHashTag(const Glib::ustring &str, size_t pos = Glib::ustring::npos);
  static bool doesAttributeMatch(const Glib::ustring &attribute, const Glib::ustring &query);

  tAttributes m_attributes;
};
