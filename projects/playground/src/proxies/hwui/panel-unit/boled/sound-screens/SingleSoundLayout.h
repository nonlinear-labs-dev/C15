#pragma once

#include <proxies/hwui/Layout.h>
#include <proxies/hwui/HWUIEnums.h>

class SingleSoundEditMenuLegacy;
class SingleSoundMenu;
class Button;
class AnyParameterLockedIndicator;

class SingleSoundLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  SingleSoundLayout(FocusAndMode focusAndMode);
  virtual ~SingleSoundLayout();

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void toggleEditMenu(bool);

  enum class Column
  {
    None,
    Init,
    Randomize,
    TranstionTime,
    Edit
  };

  void toggleColumn(Column c);
  void setup();
  void randomize();
  void action();
  void setDefault();
  void initSound();

  Button *m_initButton = nullptr;

  Control *m_randomizeLabel = nullptr;
  Control *m_randomizeSlider = nullptr;
  Control *m_randomizeAmount = nullptr;
  Button *m_randomButton = nullptr;
  Button *m_emptyButton = nullptr;

  Control *m_transitionTimeLabel = nullptr;
  Control *m_transitionTimeSlider = nullptr;
  Control *m_transitionTimeAmount = nullptr;
  Button *m_transitionTimeButton = nullptr;
  AnyParameterLockedIndicator *m_paramLocked = nullptr;

  SingleSoundEditMenuLegacy *m_initMenu = nullptr;

  Control *m_edit = nullptr;

  Column m_selectedColumn = Column::None;
};
