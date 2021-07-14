#pragma once

#include "Serializer.h"

class ParameterGroupSet;

class ParameterGroupsSerializer : public Serializer
{
 public:
  explicit ParameterGroupsSerializer(ParameterGroupSet *paramGroups);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  ParameterGroupSet *m_paramGroups;
};
