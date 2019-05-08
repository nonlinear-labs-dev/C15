#pragma once

#include "Setting.h"
#include <parameters/value/QuantizedValue.h>

class RandomizeAmount : public Setting
{
 private:
  typedef Setting super;

 public:
  RandomizeAmount(Settings &parent);
  virtual ~RandomizeAmount();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void set(tControlPositionValue amount);
  tControlPositionValue get() const;
  void setDefault();
  void incDec(int incs, ButtonModifiers mod);
  ustring getDisplayString() const;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  QuantizedValue m_amount;
};
