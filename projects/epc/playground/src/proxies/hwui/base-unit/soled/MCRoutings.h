#pragma once

#include "proxies/hwui/controls/Control.h"
#include "groups/MacroControlMappingGroup.h"

class Application;
class EditBuffer;
class Parameter;

class MCRoutings : public Control
{
 private:
  typedef Control super;

 public:
  MCRoutings(const ParameterId &ribbonParamID, const Rect &pos);
  virtual ~MCRoutings();

  bool redraw(FrameBuffer &fb) override;

 private:
  MCRoutings(const MCRoutings &other) = delete;
  MCRoutings &operator=(const MCRoutings &) = delete;

  EditBuffer *getEditBuffer() const;
  void connectToRoutingParameters();
  void onRoutingParameterChanged(const Parameter *p);

  void drawRoutingParameters(FrameBuffer &fb, MacroControlMappingGroup::tModRoutingParams &params);
  void drawRoutingParameter(FrameBuffer &fb, MacroControlMappingGroup::tModRoutingParam param, int yPos);

  MacroControlMappingGroup::tModRoutingParams getRoutingParameters();

  ParameterId m_ribbonParamID;
};
