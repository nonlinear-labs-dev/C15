#pragma once

#include <device-settings/Setting.h>
#include <parameters/value/QuantizedValue.h>
#include <playground.h>

class Settings;

class TransitionTime : public Setting
{
 private:
  typedef Setting super;

 public:
  TransitionTime(Settings &parent);
  virtual ~TransitionTime();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void incDec(int incs, ButtonModifiers mods);
  void set(tControlPositionValue amount);
  tControlPositionValue get() const;

  void setDefault();

  Glib::ustring getDisplayString() const;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  Glib::ustring getName() const
  {
    return "Transition Time";
  }

 private:
  void sendToLPC(SendReason reason) const override;

  QuantizedValue m_time;
};
