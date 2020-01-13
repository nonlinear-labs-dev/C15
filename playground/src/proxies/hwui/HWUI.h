#pragma once

#include <libundo/undo/Scope.h>
#include <playground.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/FineButton.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <nltools/threading/Expiration.h>
#include <tools/Signal.h>
#include <array>
#include <memory>
#include <giomm.h>

class Application;
class UsageMode;

namespace nltools
{
  namespace msg
  {
    struct ButtonChangedMessage;
  }
}

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

  VoiceGroup getCurrentVoiceGroup() const;

  void setCurrentVoiceGroup(VoiceGroup v);
  void setCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction, VoiceGroup v);

  void toggleCurrentVoiceGroupAndUpdateParameterSelection(UNDO::Transaction *transaction);
  void toggleCurrentVoiceGroup();
  sigc::connection onCurrentVoiceGroupChanged(const sigc::slot<void, VoiceGroup> &cb);

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

 private:
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

  PanelUnit m_panelUnit;
  BaseUnit m_baseUnit;

  Glib::RefPtr<Gio::Cancellable> m_readersCancel;
  Glib::RefPtr<Gio::DataInputStream> m_keyboardInput;

  FineButton m_fineButton;
  ButtonModifiers m_modifiers;

  sigc::connection m_blinkTimerConnection;
  Signal<void, ButtonModifiers> m_modifersChanged;
  Signal<void, int> m_blinkTimer;

  Signal<void, VoiceGroup> m_voiceGoupSignal;
  Signal<void, UNDO::Transaction *, VoiceGroup> m_voiceGroupWithTransactionSignal;

  std::array<bool, (size_t) Buttons::NUM_BUTTONS> m_buttonStates;

  int m_affengriffState = 0;

  FocusAndMode m_focusAndMode;

  int m_blinkCount;
  Expiration m_switchOffBlockingMainThreadIndicator;

  VoiceGroup m_currentVoiceGroup = VoiceGroup::I;

  bool m_focusAndModeFrozen = false;
};
