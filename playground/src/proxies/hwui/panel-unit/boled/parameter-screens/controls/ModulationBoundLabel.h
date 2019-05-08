#pragma once

#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/controls/LabelRegular8.h>

class MacroControlParameter;
class Rect;

class ModulationBoundLabel : public LabelRegular8
{
  typedef LabelRegular8 super;

 public:
  ModulationBoundLabel(const Rect &r);
  virtual ~ModulationBoundLabel();

 protected:
  virtual void updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam) = 0;
  ModulateableParameter *getModulatedParameter();

 private:
  void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);
  void onParameterChanged(const Parameter *param);
  void onMCParameterChanged(const Parameter *mcParam);
  void onButtonModifiersChanged(ButtonModifiers mod);
  void updateText();

  ModulateableParameter::ModulationSource m_mc = ModulateableParameter::NONE;
  sigc::connection m_paramConnection;
  sigc::connection m_mcConnection;

  MacroControlParameter *m_mcParam = nullptr;
  ModulateableParameter *m_modulatedParam = nullptr;
};
