#pragma once

#include "RenameLayout.h"
#include <libundo/undo/Transaction.h>

class Preset;

class EditPresetInfoLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  EditPresetInfoLayout();

 protected:
  void commit(const Glib::ustring& newName) override;
  Glib::ustring getInitialText() const override;

 protected:
  Preset* m_currentPreset;
};
