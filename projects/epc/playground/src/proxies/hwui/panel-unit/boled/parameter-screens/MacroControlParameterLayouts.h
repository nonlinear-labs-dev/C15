#pragma once

#include "ParameterLayout.h"

class MacroControlParameterLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;
  MacroControlParameterLayout2();
  ~MacroControlParameterLayout2();

  virtual void copyFrom(Layout *other) override;

 protected:
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;
  virtual Parameter *getCurrentEditParameter() const override;

  enum class Mode
  {
    MacroControlValue,
    PlayControlPosition,
    PlayControlSelection,
    PlayControlAmount
  };

  void toggleMode(Mode desiredMode);
  virtual void setMode(Mode desiredMode);
  Mode getMode() const;

  Overlay *getModeOverlay();

 protected:
  void setButtonText(Buttons b, const std::string &s);
  std::string getButtonText(Buttons b) const;
  void setButtonAText(const std::string &s);
  void setButtonA(Button *button);
  virtual Control *createMCAssignmentIndicator();

 private:
  void onSoundTypeChanged();

  Button *m_buttonA = nullptr;
  Parameter *getCurrentRouter() const;
  Parameter *getCurrentPlayControl() const;

  sigc::connection m_editBufferTypeConnection;

  Mode m_mode = Mode::MacroControlValue;
  Overlay *m_modeOverlay = nullptr;
  void selectSmoothingParameterForMC();
};

class MacroControlParameterSelectLayout2 : public ParameterSelectLayout2, public MacroControlParameterLayout2
{
 public:
  typedef ParameterSelectLayout2 super1;
  typedef MacroControlParameterLayout2 super2;
  MacroControlParameterSelectLayout2();

 protected:
  virtual void init() override;
  virtual Carousel *createCarousel(const Rect &rect) override;

  virtual void setMode(Mode desiredMode) override;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class MacroControlParameterEditLayout2 : public ParameterEditLayout2, public MacroControlParameterLayout2
{
 public:
  typedef ParameterEditLayout2 super1;
  typedef MacroControlParameterLayout2 super2;

  MacroControlParameterEditLayout2();

 protected:
  virtual void setMode(Mode desiredMode) override;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual ButtonMenu *createMenu(const Rect &rect) override;
  Control *createMCAssignmentIndicator() override;
};
