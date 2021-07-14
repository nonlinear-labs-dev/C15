#pragma once

#include <parameters/Parameter.h>
#include <nltools/threading/Throttler.h>

class ScaleParameter : public Parameter
{
  typedef Parameter super;

 public:
  ScaleParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, tControlPositionValue def,
                 tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);

  Layout *createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring getMiniParameterEditorName() const override;
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  Glib::ustring getLongName() const override;
  size_t getHash() const override;
};
