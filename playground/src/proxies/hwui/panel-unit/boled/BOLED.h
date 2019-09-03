#pragma once

#include "proxies/hwui/OLEDProxy.h"
#include "proxies/hwui/buttons.h"

class Application;
class Parameter;
class Setting;

namespace DescriptiveLayouts {
  class GenericLayout;
}

class BOLED : public OLEDProxy, public sigc::trackable
{
 public:
  BOLED();
  ~BOLED() override;

  void init();

  bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state);
  void onRotary(signed char i);

  void runPerformanceTest();

  void setupFocusAndMode(FocusAndMode focusAndMode);
  bool isSameParameterScreen(const DescriptiveLayouts::GenericLayout *layout, const FocusAndMode& focusAndMode) const;
  void showUndoScreen();

  void bruteForce();

  sigc::connection onLayoutInstantiated(const sigc::slot<void, Layout*>& s);

  void reset(Layout *layout) override;

  void reset(tLayoutPtr layout) override;

private:
  void installOldLayouts(FocusAndMode foucsAndMode);
  void setupSoundScreen(FocusAndMode focusAndMode);
  void setupParameterScreen(FocusAndMode focusAndMode);
  void setupPresetScreen(FocusAndMode focusAndMode);
  void setupBankScreen(FocusAndMode focusAndMode);

  void setupFocusAndModeMixed(FocusAndMode focusAndMode);
  void setupFocusAndModeDescriptiveLayouts(FocusAndMode focusAndMode);

  sigc::signal<void, Layout*> m_layoutInstantiated;
};
