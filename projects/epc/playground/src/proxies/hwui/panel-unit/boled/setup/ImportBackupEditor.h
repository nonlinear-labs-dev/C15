#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <filesystem>
#include <xml/XmlReader.h>

class ImportBackupEditor : public ControlWithChildren, public SetupEditor
{
 public:
  ImportBackupEditor();
  ~ImportBackupEditor() override;

  void setPosition(const Rect &) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 private:
  void addLabel(const Glib::ustring &text);
  void importBackup();
  static Glib::ustring generateFileDialogCompliantNameFromPath(std::filesystem::directory_entry file);
  static bool filterApplicableFileNames(std::filesystem::directory_entry term);
  static void importBackupFileFromPath(std::filesystem::directory_entry file);
};
