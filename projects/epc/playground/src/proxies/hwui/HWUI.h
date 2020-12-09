#pragma once

#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/FineButton.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <nltools/threading/Expiration.h>
#include <tools/Signal.h>
#include <tools/DelayedJob.h>
#include <array>
#include <memory>
#include <glibmm/refptr.h>
#include <png++/image.hpp>

class Application;
class UsageMode;

namespace UNDO
{
  class Transaction;
}

namespace nltools
{
  namespace msg
  {
    struct ButtonChangedMessage;
  }
}

class PresetPartSelection;

class HWUI
{
 public:
  HWUI();
  virtual ~HWUI();

  void init();

  void indicateBlockingMainThread();

  void undoableSetFocusAndMode(UNDO::Transaction *transaction, FocusAndMode focusAndMode);
  void setUiModeDetail(UIDetail detail);
  void undoableSetFocusAndMode(FocusAndMode focusAndMode);
  void setFocusAndMode(const UIDetail &detail);
  void setFocusAndMode(FocusAndMode focusAndMode);
  FocusAndMode getFocusAndMode() const;
  FocusAndMode getOldFocusAndMode() const;

  VoiceGroup getCurrentVoiceGroup() const;
  bool isInLoadToPart() const;

  void setLoadToPart(bool state);
  void setCurrentVoiceGroup(VoiceGroup v);
  void setCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction, VoiceGroup v);

  void toggleCurrentVoiceGroupAndUpdateParameterSelection();
  void toggleCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction);
  void toggleCurrentVoiceGroup();

  sigc::connection onCurrentVoiceGroupChanged(const sigc::slot<void, VoiceGroup> &cb);
  sigc::connection onLoadToPartModeChanged(const sigc::slot<void, bool> &cb);
  sigc::connection onHWUIChanged(const sigc::slot<void> &cb);

  void freezeFocusAndMode();
  void thawFocusAndMode();

  PanelUnit &getPanelUnit();
  const PanelUnit &getPanelUnit() const;

  BaseUnit &getBaseUnit();

  ButtonModifiers getButtonModifiers() const;
  bool isResolutionFine() const;
  void unsetFineMode();
  bool isModifierSet(ButtonModifier m) const;

  sigc::connection onModifiersChanged(const sigc::slot<void, ButtonModifiers> &cb);
  sigc::connection connectToBlinkTimer(const sigc::slot<void, int> &cb);
  void deInit();

  void toggleLoadToPart();

  PresetPartSelection *getPresetPartSelection(VoiceGroup vg);

  std::string exportSoled();
  std::string exportBoled();

 private:
  void exportOled(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const std::string &fileName) const;

  void onPresetLoaded();
  void onEditBufferSoundTypeChanged(SoundType type);
  void undoableUpdateParameterSelection(UNDO::Transaction *transaction);
  void onButtonMessage(const nltools::msg::ButtonChangedMessage &msg);
  void onButtonPressed(Buttons buttonID, bool state);

  void onKeyboardLineRead(Glib::RefPtr<Gio::AsyncResult> &res);

  void addModifier(ButtonModifier i);
  void removeModifier(ButtonModifier i);
  void setModifiers(ButtonModifiers m);

  bool onBlinkTimeout();
  void setupFocusAndMode();

  void setModifiers(Buttons buttonID, bool state);
  bool detectAffengriff(Buttons buttonID, bool state);
  bool isFineAllowed();

  static FocusAndMode fixFocusAndModeWithAnys(FocusAndMode in);
  static FocusAndMode removeInvalidsFromSound(FocusAndMode in);

  FocusAndMode restrictFocusAndMode(FocusAndMode in) const;
  FocusAndMode removeEditOnFocusChange(FocusAndMode in) const;

  sigc::connection m_editBufferSoundTypeConnection;
  sigc::connection m_editBufferPresetLoadedConnection;
  sigc::connection m_rotaryChangedConnection;

  void onRotaryChanged();

  Signal<void, VoiceGroup> m_voiceGoupSignal;
  Signal<void, bool> m_loadToPartSignal;
  Signal<void> m_inputSignal;

  bool m_loadToPartActive = false;
  VoiceGroup m_currentVoiceGroup = VoiceGroup::I;

  PanelUnit m_panelUnit;
  BaseUnit m_baseUnit;

  Glib::RefPtr<Gio::Cancellable> m_readersCancel;
  Glib::RefPtr<Gio::DataInputStream> m_keyboardInput;

  FineButton m_fineButton;
  ButtonModifiers m_modifiers;

  sigc::connection m_blinkTimerConnection;
  Signal<void, ButtonModifiers> m_modifersChanged;
  Signal<void, int> m_blinkTimer;

  std::array<bool, (size_t) Buttons::NUM_BUTTONS> m_buttonStates;

  int m_affengriffState = 0;

  FocusAndMode m_focusAndMode;
  FocusAndMode m_oldFocusAndMode;

  int m_blinkCount;
  Expiration m_switchOffBlockingMainThreadIndicator;

  bool m_focusAndModeFrozen = false;

  DelayedJob m_setupJob;
};
