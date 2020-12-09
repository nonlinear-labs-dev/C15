#pragma once
#include <proxies/hwui/controls/Overlay.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControl.h>
#include <variant>

class Button;

class ModAspectRecallOverlay : public Overlay, public ButtonReceiver
{
 public:
  enum Mode
  {
    MC_POS,
    MC_SEL,
    MC_AMT
  };

  ModAspectRecallOverlay(const Rect& r, ModulateableParameter* modP, Mode mode);
  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;
  void drawBackground(FrameBuffer& fb) override;
  int getZPriority() const override;
  bool redraw(FrameBuffer& fb) override;

 private:
  void updateUI();

  bool isMCAssignedAndChanged() const;
  bool isMCAmtChanged() const;
  bool isMCSelChanged() const;

  float getOGAmount() const;
  float getOGMCPos() const;
  MacroControls getOGMCSelection() const;

  float getMCAmount() const;
  float getMCPos() const;
  MacroControls getMCSelection() const;

  void installMode(Mode m);

  Label* m_labelA = nullptr;
  Label* m_labelB = nullptr;
  Label* m_labelC = nullptr;

  Mode m_mode;
  ModulateableParameter* m_modParam;
  Button* m_buttonA = nullptr;
  Button* m_buttonB = nullptr;
  Button* m_buttonC = nullptr;
  Button* m_buttonD = nullptr;

  struct RecallPair
  {
    bool anyRecallHappened = false;
    bool likeInPreset = false;
    std::variant<double, MacroControls> leftRecallValue;
    std::variant<double, MacroControls> rightRecallValue;
  };

  std::unordered_map<Mode, RecallPair> m_oldRecallValues;
  void doRecall();
  void undoRecall();
  void previousMode();
  void nextMode();

  std::string mcSelectionToDisplay(MacroControls src) const;
  std::string stringizeMCPos(tDisplayValue src) const;
};