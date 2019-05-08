#pragma once

#include "playground.h"
#include "InStream.h"
#include "Reader.h"
#include <glibmm/markup.h>
#include "libundo/undo/Scope.h"
#include "Attributes.h"

class Attributes;
class Serializer;

using namespace Glib::Markup;

class XmlReader : public Parser, public Reader
{
 private:
  typedef Reader super;

 public:
  XmlReader(InStream& in, UNDO::Scope::tTransactionPtr transaction);
  virtual ~XmlReader();

  static Glib::ustring trim(const Glib::ustring& text);

 protected:
  void on_start_element(ParseContext& context, const Glib::ustring& element_name, const AttributeMap& attributes);
  void on_end_element(ParseContext& context, const Glib::ustring& element_name);
  void on_text(ParseContext& context, const Glib::ustring& text);

 private:
  void feed() override;

  Markup::ParseContext m_context;

  Attributes m_lastAttributes;
  Glib::ustring m_lastTag;
  Glib::ustring m_lastText;
};
