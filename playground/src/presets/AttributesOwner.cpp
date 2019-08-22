#include "AttributesOwner.h"
#include <libundo/undo/Transaction.h>
#include <xml/Writer.h>
#include <unordered_set>
#include <device-settings/DebugLevel.h>

AttributesOwner::AttributesOwner(UpdateDocumentContributor *parent)
    : UpdateDocumentContributor(parent)
{
}

AttributesOwner::AttributesOwner(UpdateDocumentContributor *parent, const AttributesOwner *other)
    : AttributesOwner(parent)
{
  m_attributes = other->m_attributes;
}

AttributesOwner::~AttributesOwner()
{
}

size_t AttributesOwner::getHash() const
{
  size_t hash = 0;

  for(auto &a : m_attributes)
  {
    hash_combine(hash, a.first);
    hash_combine(hash, a.second);
  }

  return hash;
}

void AttributesOwner::setAttribute(UNDO::Transaction *transaction, const std::string &key, const Glib::ustring &value)
{
  auto swapData = UNDO::createSwapData(value);

  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    auto curValue = getAttribute(key, "");
    swapData->swapWith(curValue);

    if(curValue.empty())
      m_attributes.erase(key);
    else
      m_attributes[key] = curValue;

    onChange();
  });
}

void AttributesOwner::copyFrom(UNDO::Transaction *transaction, const AttributesOwner *other)
{
  clear(transaction);

  for(auto &a : other->m_attributes)
    setAttribute(transaction, a.first, a.second);
}

void AttributesOwner::clear(UNDO::Transaction *transaction)
{
  transaction->addUndoSwap(this, m_attributes, tAttributes());
}

Glib::ustring AttributesOwner::getAttribute(const Glib::ustring &key, const Glib::ustring &def) const
{
  auto it = m_attributes.find(key);

  if(it != m_attributes.end())
    return it->second;

  return def;
}

const AttributesOwner::tAttributes &AttributesOwner::getAttributes() const
{
  return m_attributes;
}

void AttributesOwner::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  for(auto &it : m_attributes)
    writer.writeTextElement("attribute", it.second, Attribute("key", it.first));
}

void AttributesOwner::writeDiff(Writer &writer, const AttributesOwner *other) const
{
  std::unordered_set<std::string> keys;

  for(auto &a : m_attributes)
    keys.insert(a.first);

  for(auto &a : other->m_attributes)
    keys.insert(a.first);

  for(const auto& key : keys)
  {
    auto va = getAttribute(key, "");
    auto vb = other->getAttribute(key, "");

    if(va != vb)
    {
      writer.writeTextElement(key, "", Attribute("a", va), Attribute("b", vb));
    }
  }
}
