#pragma once

#include "BooleanSetting.h"

class SendPresetAsPlaycontrollerWriteFallback : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  explicit SendPresetAsPlaycontrollerWriteFallback(UpdateDocumentContributor& settings);
  ~SendPresetAsPlaycontrollerWriteFallback() override;

  SendPresetAsPlaycontrollerWriteFallback(const SendPresetAsPlaycontrollerWriteFallback& other) = delete;
  SendPresetAsPlaycontrollerWriteFallback& operator=(const SendPresetAsPlaycontrollerWriteFallback&) = delete;
};
