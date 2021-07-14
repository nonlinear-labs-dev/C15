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
  explicit SelectedMacroControlsHWSourceAmount(const Rect& rect);
  ~SelectedMacroControlsHWSourceAmount() override;
  SelectedMacroControlsHWSourceAmount(const SelectedMacroControlsHWSourceAmount& other) = delete;
  SelectedMacroControlsHWSourceAmount& operator=(const SelectedMacroControlsHWSourceAmount&) = delete;

 private:
  void onParameterSelected(Parameter* newOne);
  void onMCChanged(const Parameter* param);
  void updateText(const Parameter* param);
  void updateTextFromRouter(const Parameter* param);
  void onModifiersChanged();
  void setSuffixFontColor(FrameBuffer& fb) const override;
  EditBuffer* getEditBuffer();

  ParameterId m_hwParamID;
  Parameter* m_router = nullptr;
  sigc::connection m_mcChanged;
  sigc::connection m_amountChanged;
  sigc::connection m_srcChanged;
};
