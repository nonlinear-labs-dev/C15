#include "xml/Attributes.h"
#include "AttributesOwnerSerializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"

AttributesOwnerSerializer::AttributesOwnerSerializer(AttributesOwner *owner)
    : Serializer(getTagName())
    , m_owner(owner)
{
}

Glib::ustring AttributesOwnerSerializer::getTagName()
{
  return "attributes";
}

void AttributesOwnerSerializer::readProlog(Reader &reader) const
{
  m_owner->undoableClearAttributes(reader.getTransaction());
}

void AttributesOwnerSerializer::writeTagContent(Writer &writer) const
{
  for(const auto &a : m_owner->getAttributes())
  {
    writer.writeTextElement("attribute", a.second, Attribute("name", a.first));
  }
}

void AttributesOwnerSerializer::readTagContent(Reader &reader) const
{
  reader.onTextElement("attribute", [&](const Glib::ustring &text, const Attributes &attr) mutable {
    m_owner->undoableSetAttribute(reader.getTransaction(), attr.get("name"), text);
  });
}
