#pragma once

#include "Setting.h"
#include <parameters/value/QuantizedValue.h>

class RandomizeAmount : public Setting
{
 private:
  typedef Setting super;

 public:
  explicit RandomizeAmount(Settings &parent);
  ~RandomizeAmount() override;

  void load(const Glib::ustring &text, Initiator initiator) override;
  Glib::ustring save() const override;

  void set(tControlPositionValue amount);
  tControlPositionValue get() const;
  void setDefault();
  void incDec(int incs, ButtonModifiers mod);
  Glib::ustring getDisplayString() const override;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  QuantizedValue m_amount;
};
