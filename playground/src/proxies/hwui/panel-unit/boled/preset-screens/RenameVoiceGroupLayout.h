#pragma once

#include "RenameLayout.h"

class RenameVoiceGroupLayout : public RenameLayout
{
 private:
  typedef RenameLayout super;

 public:
  RenameVoiceGroupLayout(VoiceGroup vg);

 private:
  void commit(const Glib::ustring& newName) override;
  Glib::ustring getInitialText() const override;

protected:
  void cancel() override;

private:

  const VoiceGroup m_targetGroup;
};