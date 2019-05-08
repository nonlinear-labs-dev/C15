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
  virtual void commit(const Glib::ustring &newName) override;
  virtual Glib::ustring getInitialText() const override;

 private:
  shared_ptr<Preset> m_currentPreset;
};
