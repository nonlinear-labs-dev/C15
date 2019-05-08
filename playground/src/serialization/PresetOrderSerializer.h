#pragma once

#include "Serializer.h"

class PresetManager;

class PresetOrderSerializer : public Serializer
{
 public:
  PresetOrderSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs);

  static Glib::ustring getTagName();
  void finalize(Reader &reader);

 protected:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  shared_ptr<PresetBank> m_bank;
  bool m_ignoreUUIDs;
};
