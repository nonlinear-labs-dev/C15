#pragma once

#include "BooleanSetting.h"

class PresetDragDropEnabled : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  explicit PresetDragDropEnabled(UpdateDocumentContributor &settings);
  ~PresetDragDropEnabled() override;
};
