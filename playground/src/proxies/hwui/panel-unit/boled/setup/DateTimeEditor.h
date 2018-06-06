#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>
#include <tools/Expiration.h>
#include <array>

class DateTimeEditor : public ControlWithChildren, public SetupEditor
{
  public:
    DateTimeEditor ();
    virtual ~DateTimeEditor ();

    void setPosition (const Rect &) override;
    bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    bool onRotary (int inc, ButtonModifiers modifiers) override;

  private:
    void setTimeValues();

    enum Selection
    {
      Month = 0, Day = 1, Year = 2, Hour = 3, Minute = 4, NumFields
    };

    Selection m_selection = Selection::Hour;
    std::array<Label*, Selection::NumFields> m_labels;
    std::array<Label*, Selection::NumFields> m_controls;
    uint64_t m_diff = 0;
    std::time_t m_originalTime;
};

