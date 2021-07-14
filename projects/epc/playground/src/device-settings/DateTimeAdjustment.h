#pragma once

#include "Setting.h"

class DateTimeAdjustment : public Setting
{
 private:
  typedef Setting super;

 public:
  explicit DateTimeAdjustment(Settings& parent);
  ~DateTimeAdjustment() override;

  Glib::ustring getDisplayString() const override;

  void load(const Glib::ustring& text, Initiator initiator) override;
  Glib::ustring save() const override;

  void set(int64_t adjustment);
  int64_t get() const;

  void adjust(time_t modifiedTime);

  DateTimeAdjustment(const DateTimeAdjustment& other) = delete;
  DateTimeAdjustment& operator=(const DateTimeAdjustment&) = delete;

 private:
  int64_t m_adjustment = 0;
};
