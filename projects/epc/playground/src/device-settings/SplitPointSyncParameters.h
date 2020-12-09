#pragma once
#include <libundo/undo/Transaction.h>
#include "Setting.h"

class Preset;

class SplitPointSyncParameters : public Setting
{
 public:
  using tSlot = sigc::slot<void, const Setting*, UNDO::Transaction*>;

  explicit SplitPointSyncParameters(Settings& p);

  void load(const Glib::ustring& text, Initiator initiator) override;
  Glib::ustring save() const override;
  Glib::ustring getDisplayString() const override;

  bool get() const;
  void setState(bool s);
  
 private:
  bool m_state = true;
};
