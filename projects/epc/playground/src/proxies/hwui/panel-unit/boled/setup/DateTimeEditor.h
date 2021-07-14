#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>
#include <nltools/threading/Expiration.h>
#include <array>

class DateTimeEditor : public ControlWithChildren, public SetupEditor
{
 public:
  DateTimeEditor();
  ~DateTimeEditor() override;

  void setPosition(const Rect &) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void setTimeValues();

  enum Selection
  {
    Year = 0,
    Month = 1,
    Day = 2,
    Hour = 3,
    Minute = 4,
    NumFields
  };

  const Selection step(Selection s, int inc = 1) const;

  Selection m_selection = Selection::Hour;
  std::array<Label *, Selection::NumFields> m_labels {};
  std::array<Label *, Selection::NumFields> m_controls {};
  uint64_t m_diff = 0;
  std::time_t m_originalTime;

  const Rect c_fullRightSidePosition { 129, 16, 126, 48 };
};
