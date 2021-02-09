#pragma once

#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/controls/LabelRegular8.h>

class MacroControlParameter;
class Rect;

class ModulationBoundLabel : public LabelRegular8
{
  typedef LabelRegular8 super;

 public:
  explicit ModulationBoundLabel(const Rect &r);
  ~ModulationBoundLabel() override;

 protected:
  virtual void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) = 0;
  ModulateableParameter *getModulatedParameter();

 private:
  void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam, SignalOrigin signalType);
  void onParameterChanged(const Parameter *param);
  void onMCParameterChanged(const Parameter *mcParam);
  void onButtonModifiersChanged(ButtonModifiers mod);
  void updateText();
  void setSuffixFontColor(FrameBuffer &fb) const override;
  StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const override;

  MacroControls m_mc = MacroControls::NONE;
  sigc::connection m_paramConnection;
  sigc::connection m_mcConnection;

  MacroControlParameter *m_mcParam = nullptr;
  ModulateableParameter *m_modulatedParam = nullptr;
};
