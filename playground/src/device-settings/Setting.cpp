#include "Setting.h"
#include "Settings.h"
#include "xml/Writer.h"

Setting::Setting(Settings &parent)
    : super(&parent)
{
}

Setting::~Setting()
{
}

connection Setting::onChange(slot<void, const Setting *> slot)
{
  return m_signal.connectAndInit(slot, this);
}

void Setting::notify()
{
  m_signal.send(this);
  super::onChange();
  sendToLPC();
}

void Setting::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();
  writer.writeTextElement("value", save(), Attribute("changed", changed));
}

void Setting::sendToLPC() const
{
}

void Setting::setSetting(Initiator initiator, const Glib::ustring &text)
{
  load(text);
}
