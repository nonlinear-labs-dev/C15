#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <proxies/hwui/HWUIEnums.h>

class SingleSoundEditMenu;
class SingleSoundMenu;

class SingleSoundLayout : public DFBLayout
{
  private:
    typedef DFBLayout super;

  public:
    SingleSoundLayout (FocusAndMode focusAndMode);
    virtual ~SingleSoundLayout ();

    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual bool onRotary (int inc, ButtonModifiers modifiers) override;

  private:
    enum class Column
    {
      None, Init, Randomize, TranstionTime, Convert, Edit
    };

    void toggleColumn (Column c);
    void setup ();
    void randomize ();
    void action ();
    void setDefault ();
    void initSound ();

    Button *m_initButton = nullptr;

    Control *m_randomizeLabel = nullptr;
    Control *m_randomizeSlider = nullptr;
    Control *m_randomizeAmount = nullptr;
    Button *m_randomButton = nullptr;

    Control *m_transitionTimeLabel = nullptr;
    Control *m_transitionTimeSlider = nullptr;
    Control *m_transitionTimeAmount = nullptr;
    Button *m_transitionTimeButton = nullptr;

    SingleSoundMenu *m_convertMenu = nullptr;
    SingleSoundEditMenu *m_initMenu = nullptr;

    Control *m_edit = nullptr;

    Column m_selectedColumn = Column::None;
};

