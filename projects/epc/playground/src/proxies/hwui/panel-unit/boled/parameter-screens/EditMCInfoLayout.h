#pragma once

#include <proxies/hwui/panel-unit/boled/preset-screens/RenameLayout.h>

class MacroControlParameter;

class EditMCInfoLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  EditMCInfoLayout();

 private:
  void commit(const Glib::ustring &newName) override;
  Glib::ustring getInitialText() const override;

  MacroControlParameter *getMacroControl() const;
};
