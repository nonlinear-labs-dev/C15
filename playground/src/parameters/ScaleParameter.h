#pragma once

#include <parameters/Parameter.h>
#include <tools/Throttler.h>

class ScaleParameter : public Parameter
{
  typedef Parameter super;

 public:
  ScaleParameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tControlPositionValue def,
                 tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator);

  DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring getMiniParameterEditorName() const override;
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  virtual Glib::ustring getLongName() const override;
  virtual size_t getHash() const override;
};
