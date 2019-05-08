#pragma once

#include <proxies/hwui/panel-unit/boled/preset-screens/RenameLayout.h>
#include <presets/PresetBank.h>

class RenameExportLayout : public RenameLayout
{
 public:
  RenameExportLayout(std::shared_ptr<PresetBank> bank,
                     std::function<void(Glib::ustring, std::shared_ptr<PresetBank>)> callBack);
  virtual ~RenameExportLayout();
  Glib::ustring getInitialText() const;
  void commit(const Glib::ustring &newName);

 private:
  Glib::ustring initialName;
  std::shared_ptr<PresetBank> m_bank;
  std::function<void(Glib::ustring, std::shared_ptr<PresetBank>)> callBackAfterCommit;
};
