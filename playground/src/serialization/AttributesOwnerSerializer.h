#pragma once

#include "presets/AttributesOwner.h"
#include "Serializer.h"

class AttributesOwnerSerializer : public Serializer
{
 public:
  AttributesOwnerSerializer(AttributesOwner *owner);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;
  void readProlog(Reader &reader) const override;

  AttributesOwner *m_owner;
};
