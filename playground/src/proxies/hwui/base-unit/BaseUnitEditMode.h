#pragma once

#include "BaseUnitPlayMode.h"

class Application;

class BaseUnitEditMode : public BaseUnitPlayMode
{
 private:
  typedef BaseUnitPlayMode super;

 public:
  BaseUnitEditMode();
  virtual ~BaseUnitEditMode();

 protected:
  void toggleTouchBehaviour() override;

 private:
  BaseUnitEditMode(const BaseUnitEditMode& other) = delete;
  BaseUnitEditMode& operator=(const BaseUnitEditMode&) = delete;

  void toggleParameterEditModeRibbonBehaviour();
};
