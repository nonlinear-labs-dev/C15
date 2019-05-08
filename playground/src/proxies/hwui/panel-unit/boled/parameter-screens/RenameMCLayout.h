#pragma once

#include <proxies/hwui/panel-unit/boled/preset-screens/RenameLayout.h>

class MacroControlParameter;

class RenameMCLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  RenameMCLayout();

 private:
  virtual void commit(const Glib::ustring &newName) override;
  virtual Glib::ustring getInitialText() const override;

  MacroControlParameter *getMacroControl() const;
};
