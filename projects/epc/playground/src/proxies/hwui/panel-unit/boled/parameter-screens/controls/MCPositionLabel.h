#pragma once

#include "proxies/hwui/controls/Label.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class MCPositionLabel : public Label
{
 private:
  typedef Label super;

 public:
  MCPositionLabel(const Rect &rect);
  virtual ~MCPositionLabel();

 private:
  MCPositionLabel(const MCPositionLabel &other);
  MCPositionLabel &operator=(const MCPositionLabel &);

  void onParameterSelected(Parameter *newParameter, SignalOrigin signalType);
  void updateTarget(const Parameter *parameter);
  void updateSource(const Parameter *parameter);
  void onModifiersChanged();

  virtual void setSuffixFontColor(FrameBuffer &fb) const override;
  void ensureDisconnectedModulationSourceIfApplicable(const Parameter *parameter);
  sigc::connection m_paramValueConnection;
  sigc::connection m_mcValueConnection;
};
