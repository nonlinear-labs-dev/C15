#pragma once

#include "RenameLayout.h"

class RenameVoiceGroupLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  explicit RenameVoiceGroupLayout(VoiceGroup vg);

 protected:
  void cancel() override;

 private:
  void commit(const Glib::ustring& newName) override;
  Glib::ustring getInitialText() const override;
  const VoiceGroup m_targetGroup;
};