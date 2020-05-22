#pragma once

#include "Setting.h"
#include <parameters/value/QuantizedValue.h>

class EncoderAcceleration : public Setting
{
 private:
  typedef Setting super;

 public:
  EncoderAcceleration(Settings &parent);
  virtual ~EncoderAcceleration();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void set(tControlPositionValue amount);
  tControlPositionValue get() const;
  void setDefault();
  void incDec(int incs, ButtonModifiers modifiers);
  Glib::ustring getDisplayString() const;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  QuantizedValue m_acceleration;

  void sendToLPC(SendReason reason) const override;
};
