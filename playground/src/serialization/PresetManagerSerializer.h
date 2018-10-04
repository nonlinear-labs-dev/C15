#pragma once

#include "Serializer.h"

class PresetManager;

class PresetManagerSerializer : public Serializer
{
  public:
    PresetManagerSerializer (PresetManager &pm);
    ~PresetManagerSerializer();

    static Glib::ustring getTagName();

  private:
    void writeTagContent(Writer &writer) const;
    void readTagContent(Reader &reader) const;

    PresetManager &m_pm;
};
