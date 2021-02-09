#pragma once

#include "ParameterLayout.h"

class ModAspectRecallOverlay;

class ModulateableParameterLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;
  ModulateableParameterLayout2();
  virtual bool switchToNormalMode();

 protected:
  void addModAmountSliders(ControlOwner *o);
};

class ModulateableParameterSelectLayout2 : public ParameterSelectLayout2, public ModulateableParameterLayout2
{
 public:
  typedef ParameterSelectLayout2 super1;
  typedef ModulateableParameterLayout2 super2;
  ModulateableParameterSelectLayout2();

  virtual void copyFrom(Layout *other) override;
  void installMcAmountScreen();

 protected:
  virtual void init() override;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;
  virtual Parameter *getCurrentEditParameter() const override;
  virtual void setDefault() override;
  virtual bool switchToNormalMode() override;
  void handlePresetValueRecall() override;

 private:
  void handleSelectPartButton();
  void onModfiersChanged(ButtonModifiers modifiers);
  void onSelectedParameterChanged(Parameter *, Parameter *newParam, SignalOrigin signalType);
  void onCurrentParameterChanged(const Parameter *p);
  void fixModeIfNecessary(const Parameter *p);

 protected:
  enum class Mode
  {
    ParameterValue,
    MacroControlPosition,
    MacroControlSelection,
    MacroControlAmount,
    CarouselUpperBound,
    CarouselParameterValue,
    CarouselLowerBound,
    Recall
  };

  virtual void setMode(Mode desiredMode);
  Mode getMode() const;
  void toggleMode(Mode desiredMode);
  bool isModeOf(std::vector<Mode> modes) const;

 private:
  bool hasModulationSource() const;
  Button *m_mcPosButton, *m_mcSelButton, *m_mcAmtButton;

  Mode m_mode = Mode::ParameterValue;
  Mode m_lastMode = Mode::ParameterValue;
  Overlay *m_modeOverlay = nullptr;
  ModAspectRecallOverlay *m_modAspectRecallOverlay = nullptr;
  sigc::connection m_paramConnection;

  void installModulationCarousel(const Mode &mode);
  bool isCurrentParameterDisabled() const;
  void installModAspectRecall();
  void removeModAspectRecall();
  void cleanMode();
};

class ModulateableParameterEditLayout2 : public ParameterEditLayout2, public ModulateableParameterLayout2
{
 public:
  typedef ParameterEditLayout2 super1;
  typedef ModulateableParameterLayout2 super2;

  ModulateableParameterEditLayout2();
  virtual ButtonMenu *createMenu(const Rect &rect) override;
};
