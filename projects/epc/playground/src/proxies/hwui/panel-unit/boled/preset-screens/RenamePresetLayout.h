#pragma once

#include "RenameLayout.h"

class Preset;

class RenamePresetLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  typedef std::function<void(const Glib::ustring &)> tCommitCB;
  typedef std::function<void(void)> tCancelCB;
  explicit RenamePresetLayout(tCommitCB commitCB, tCancelCB cancelCB = nullptr);

 protected:
  void commit(const Glib::ustring &newName) override;
  void cancel() override;
  Glib::ustring getInitialText() const override;

 private:
  Preset *m_currentPreset;
  tCommitCB m_commitCB;
  tCancelCB m_cancelCB;
};
