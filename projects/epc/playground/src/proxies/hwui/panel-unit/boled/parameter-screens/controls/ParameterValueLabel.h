#pragma once

#include <proxies/hwui/controls/Label.h>
#include <sigc++/connection.h>
#include <nltools/Types.h>

class Parameter;

class ParameterValueLabel : public Label
{
 private:
  typedef Label super;

 public:
  ParameterValueLabel(const Parameter *param, const Rect &pos);
  ~ParameterValueLabel() override;
  void updateParameter(const Parameter *param);

 private:
  void updateText(const Parameter *param);
  void updateVoiceGroup(VoiceGroup newVoiceGroup);
  sigc::connection m_connection;
  sigc::connection m_vgSelectionConnection;
  const Parameter *m_currentParameter;
};
