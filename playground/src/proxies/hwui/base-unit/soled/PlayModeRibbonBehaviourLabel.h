#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class EditBuffer;
class Parameter;

class PlayModeRibbonBehaviourLabel : public Label
{
 private:
  typedef Label super;

 public:
  PlayModeRibbonBehaviourLabel(int paramID, const Rect& rect);
  virtual ~PlayModeRibbonBehaviourLabel();

 private:
  PlayModeRibbonBehaviourLabel(const PlayModeRibbonBehaviourLabel& other) = delete;
  PlayModeRibbonBehaviourLabel& operator=(const PlayModeRibbonBehaviourLabel&) = delete;

  void updateText(const Parameter* s);
  shared_ptr<EditBuffer> getEditBuffer() const;
};
