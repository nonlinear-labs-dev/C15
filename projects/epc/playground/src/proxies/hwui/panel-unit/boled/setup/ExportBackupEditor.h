#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Button.h>

class OutStream;

class ExportBackupEditor : public ControlWithChildren, public SetupEditor
{
 public:
  ExportBackupEditor();
  ~ExportBackupEditor() override;

  void setPosition(const Rect&) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void exportBanks();

  static void writeBackupToStream(OutStream& stream);

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
