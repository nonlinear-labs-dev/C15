#pragma once

#include "Setting.h"

class NoteShift : public Setting
{
 private:
  typedef Setting super;

 public:
  explicit NoteShift(Settings& parent);
  ~NoteShift() override;

  void load(const Glib::ustring& text, Initiator initiator) override;
  Glib::ustring save() const override;

  void set(int shift);
  int get() const;

  void incSemiTone();
  void decSemiTone();

  void incOctave();
  void decOctave();

  Glib::ustring getDisplayString() const override;
  void syncExternals(SendReason reason) const override;

  NoteShift(const NoteShift& other) = delete;
  NoteShift& operator=(const NoteShift&) = delete;

 private:
  int m_shift;
};
