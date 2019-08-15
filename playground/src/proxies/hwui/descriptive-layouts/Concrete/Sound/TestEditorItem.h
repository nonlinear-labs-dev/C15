#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/EditorItem.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/Setting.h>

class TestEditorItem : public EditorItem
{
 public:
  explicit TestEditorItem()
      : EditorItem("Test Editor")
  {
    addControl(new LeftAlignedLabel("...", { 128, 0, 128, 15 }));
  }

  void doAction() override;

  ~TestEditorItem()
  = default;

  GenericMenuEditor *createEditor() override;
};
