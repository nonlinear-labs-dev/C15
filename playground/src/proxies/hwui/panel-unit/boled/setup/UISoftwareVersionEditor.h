#pragma once

#include "DisplayEditor.h"

class UISoftwareVersionEditor : public DisplayEditor
{
 public:
  UISoftwareVersionEditor();
  virtual void setPosition(const Rect &) override;
};
