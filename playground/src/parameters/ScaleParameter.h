#pragma once

#include <parameters/Parameter.h>

class ScaleParameter : public Parameter
{
  typedef Parameter super;

 public:
  using super::super;

  DFBLayout *createLayout(FocusAndMode focusAndMode) const override;
  Glib::ustring getMiniParameterEditorName() const override;
  void writeDocProperties(Writer &writer, tUpdateID knownRevision) const override;
  virtual Glib::ustring getLongName() const override;
  virtual size_t getHash() const override;
};
