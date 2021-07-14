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
  explicit MCPositionLabel(const Rect &rect);
  ~MCPositionLabel() override;

  MCPositionLabel(const MCPositionLabel &other) = delete;
  MCPositionLabel &operator=(const MCPositionLabel &) = delete;

 private:
  void onParameterSelected(Parameter *newParameter);
  void updateTarget(const Parameter *parameter);
  void updateSource(const Parameter *parameter);
  void onModifiersChanged();

  void setSuffixFontColor(FrameBuffer &fb) const override;
  void ensureDisconnectedModulationSourceIfApplicable(const Parameter *parameter);
  sigc::connection m_paramValueConnection;
  sigc::connection m_mcValueConnection;
};
