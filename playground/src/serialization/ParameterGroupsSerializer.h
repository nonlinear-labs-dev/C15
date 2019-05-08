#pragma once

#include "Serializer.h"

class ParameterGroupSet;

class ParameterGroupsSerializer : public Serializer
{
 public:
  ParameterGroupsSerializer(ParameterGroupSet *paramGroups);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  ParameterGroupSet *m_paramGroups;
};
