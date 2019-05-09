#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <experimental/filesystem>
#include <xml/XmlReader.h>
#include <xml/VersionAttribute.h>

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
  static Glib::ustring generateFileDialogCompliantNameFromPath(std::experimental::filesystem::directory_entry file);
  static bool filterApplicableFileNames(std::experimental::filesystem::directory_entry term);
  static void importBackupFileFromPath(std::experimental::filesystem::directory_entry file);

  static Reader::FileVersionCheckResult checkForCompatibleVersion(int version);
};
