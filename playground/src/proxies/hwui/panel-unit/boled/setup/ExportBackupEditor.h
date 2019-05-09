#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Button.h>

class ExportBackupEditor : public ControlWithChildren, public SetupEditor
{
 public:
  ExportBackupEditor();
  virtual ~ExportBackupEditor();

  void setPosition(const Rect &) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void exportBanks();

  static void writeBackupToStream(std::shared_ptr<OutStream> stream);

 private:
  enum State
  {
    Initial,
    Running,
    Finished,
    NotReady
  };

  void installState(State s);
  void writeBackupFileXML();

  State m_state = Initial;
};
