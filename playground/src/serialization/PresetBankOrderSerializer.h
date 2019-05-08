#pragma once

#include "Serializer.h"

class PresetManager;

class PresetBankOrderSerializer : public Serializer
{
 public:
  PresetBankOrderSerializer(PresetManager &pm);

  static Glib::ustring getTagName();
  void finalize(Reader &reader);

 protected:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  PresetManager &m_pm;
};
