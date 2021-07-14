#pragma once

#include "Setting.h"
#include <parameters/value/QuantizedValue.h>

class RibbonRelativeFactor : public Setting
{
 private:
  typedef Setting super;

 public:
  explicit RibbonRelativeFactor(Settings &parent);
  ~RibbonRelativeFactor() override;

  void load(const Glib::ustring &text, Initiator initiator) override;
  Glib::ustring save() const override;

  void set(tControlPositionValue amount);
  tControlPositionValue get() const;
  void setDefault();
  void incDec(int incs, ButtonModifiers modifiers);
  Glib::ustring getDisplayString() const override;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void syncExternals(SendReason reason) const override;

 private:
  QuantizedValue m_factor;
};
