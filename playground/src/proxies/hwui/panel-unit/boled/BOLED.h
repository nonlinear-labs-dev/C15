#pragma once

#include "proxies/hwui/OLEDProxy.h"
#include "proxies/hwui/buttons.h"

class Application;
class Parameter;
class Setting;

class BOLED : public OLEDProxy, public sigc::trackable
{
 public:
  BOLED();
  ~BOLED() override;

  void init();

  bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state);
  void onRotary(signed char i);
  void setupFocusAndMode(FocusAndMode focusAndMode);
  void showUndoScreen();

  void bruteForce();

 private:
  void installOldLayouts(FocusAndMode foucsAndMode);
  void setupSoundScreen(FocusAndMode focusAndMode);
  void setupParameterScreen(FocusAndMode focusAndMode);
  void setupPresetScreen(FocusAndMode focusAndMode);
  void setupBankScreen(FocusAndMode focusAndMode);
};
