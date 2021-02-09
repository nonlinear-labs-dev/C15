#pragma once

#include "proxies/hwui/controls/Label.h"
#include <ParameterId.h>
#include <sigc++/connection.h>

class Application;
class Parameter;
class EditBuffer;

class SelectedMacroControlsHWSourceAmount : public Label
{
 private:
  typedef Label super;

 public:
  SelectedMacroControlsHWSourceAmount(const Rect& rect);
  virtual ~SelectedMacroControlsHWSourceAmount();

 private:
  SelectedMacroControlsHWSourceAmount(const SelectedMacroControlsHWSourceAmount& other);
  SelectedMacroControlsHWSourceAmount& operator=(const SelectedMacroControlsHWSourceAmount&);

  void onParameterSelected(Parameter* newOne, SignalOrigin signalType);
  void onMCChanged(const Parameter* param);
  void updateText(const Parameter* param);
  void updateTextFromRouter(const Parameter* param);
  void onModifiersChanged();
  EditBuffer* getEditBuffer();

  virtual void setSuffixFontColor(FrameBuffer& fb) const override;

  ParameterId m_hwParamID;
  Parameter* m_router = nullptr;
  sigc::connection m_mcChanged;
  sigc::connection m_amountChanged;
  sigc::connection m_srcChanged;
};
