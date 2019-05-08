#pragma once

#include "Serializer.h"

class LastLoadedPresetInfo;

class LastLoadedPresetInfoSerializer : public Serializer
{
 public:
  LastLoadedPresetInfoSerializer(LastLoadedPresetInfo &info);
  virtual ~LastLoadedPresetInfoSerializer();

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  LastLoadedPresetInfo &m_info;
};
