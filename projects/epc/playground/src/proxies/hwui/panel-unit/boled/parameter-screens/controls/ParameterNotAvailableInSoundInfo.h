#pragma once
#include <sigc++/connection.h>
#include <proxies/hwui/controls/ControlWithChildren.h>

class Parameter;
class ParameterLayout2;
class Label;

class ParameterNotAvailableInSoundInfo : public ControlWithChildren
{
 public:
  ParameterNotAvailableInSoundInfo(const Rect& r);
  void setVisible(bool b) override;
  int getZPriority() const override;

 protected:
  void setBackgroundColor(FrameBuffer& fb) const override;

 private:
  void onSelectionChanged(const Parameter* old, const Parameter* newParam, SignalOrigin signalType);
  void onSoundTypeChanged();

  Label* m_parameterNameLabel;

  sigc::connection m_parameterSelectionConnection;
  sigc::connection m_soundTypeConnection;
};
