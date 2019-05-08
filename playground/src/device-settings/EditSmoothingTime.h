#pragma once

#include <device-settings/Setting.h>
#include <parameters/value/QuantizedValue.h>
#include <playground.h>

class Settings;

class EditSmoothingTime : public Setting
{
 private:
  typedef Setting super;

 public:
  EditSmoothingTime(Settings &parent);
  virtual ~EditSmoothingTime();

  void load(const Glib::ustring &text) override;
  Glib::ustring save() const override;

  void incDec(int incs, ButtonModifiers mods);
  void set(tControlPositionValue amount);
  tControlPositionValue get() const;

  void setDefault();

  ustring getDisplayString() const;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  void sendToLPC() const override;

  QuantizedValue m_time;
};
