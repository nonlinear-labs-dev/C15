#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"

class Preset;

class PresetEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  enum Actions
  {
    Rename,
    Cut,
    Copy,
    Paste,
    Delete
  };

  explicit PresetEditButtonMenu(const Rect& rect);
  ~PresetEditButtonMenu() override;

  PresetEditButtonMenu(const PresetEditButtonMenu& other) = delete;
  PresetEditButtonMenu& operator=(const PresetEditButtonMenu&) = delete;

  void selectButton(size_t i) override;

 private:
  void onClipboardChanged();

  void renamePreset();
  void cutPreset();
  void copyPreset();
  void pasteClipboard();
  void deletePreset();

  static int s_lastSelection;
  Actions m_lastCopyingAction = Copy;
};
