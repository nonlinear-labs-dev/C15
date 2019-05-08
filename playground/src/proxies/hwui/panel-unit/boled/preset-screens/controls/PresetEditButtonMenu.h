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

  PresetEditButtonMenu(const Rect& rect);
  virtual ~PresetEditButtonMenu();

  void selectButton(size_t i) override;

 private:
  PresetEditButtonMenu(const PresetEditButtonMenu& other);
  PresetEditButtonMenu& operator=(const PresetEditButtonMenu&);

  void onClipboardChanged();

  void renamePreset();
  void cutPreset();
  void copyPreset();
  void pasteClipboard();
  void deletePreset();

  static int s_lastSelection;
  Actions m_lastCopyingAction = Copy;
};
