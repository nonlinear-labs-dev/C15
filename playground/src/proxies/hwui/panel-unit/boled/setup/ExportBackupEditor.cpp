#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/boled/setup/ExportBackupEditor.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <serialization/PresetManagerSerializer.h>
#include <xml/FileOutStream.h>
#include <xml/VersionAttribute.h>
#include <tools/SpawnCommandLine.h>
#include "USBStickAvailableView.h"

static const Rect c_fullRightSidePosition (129, 16, 126, 48);
static constexpr const char c_tempBackupFile[] = "/nonlinear/nonlinear-c15-banks.xml.tar.gz";
static constexpr const char c_backupTargetFile[] = "nonlinear-c15-banks.xml.tar.gz";

ExportBackupEditor::ExportBackupEditor () :
    ControlWithChildren (Rect (0, 0, 0, 0))
{
  if (USBStickAvailableView::usbIsReady())
  installState (Initial);
  else
    installState (NotReady);
}

ExportBackupEditor::~ExportBackupEditor ()
{
}

void ExportBackupEditor::installState (State s)
{
  m_state = s;

  clear ();

  switch (m_state)
  {
  case NotReady:
    addControl(new MultiLineLabel("Please insert an USB-Stick and reload this page!"))->setPosition(Rect(3, 0, 119, 24));
    addControl(new Button("Back", Button::getButtonPos(Buttons::BUTTON_D).getMovedBy(Point(-128, -16))));
    break;
  case Initial:
    addControl(new MultiLineLabel("Save all banks to USB? This might take a while, and will freeze your C15."))->setPosition(
        Rect(3, 0, 119, 24));
    addControl(new Button("OK", Button::getButtonPos(Buttons::BUTTON_C).getMovedBy(Point(-128, -16))));
    addControl(new Button("Cancel", Button::getButtonPos(Buttons::BUTTON_D).getMovedBy(Point(-128, -16))));
    break;

  case Running:
    addControl(new Label("Saving Banks, stand by...", Rect(3, -20, 128, 64)));
    Oleds::get().syncRedraw();
    exportBanks();
    break;

  case Finished:
    addControl(new Label("Saved all banks to USB!", Rect(3, -20, 128, 64)));
    addControl(new Button("Save again!", Button::getButtonPos(Buttons::BUTTON_C).getMovedBy(Point(-128, -16))));
    addControl(new Button("Done", Button::getButtonPos(Buttons::BUTTON_D).getMovedBy(Point(-128, -16))));
    break;
  }
}

void ExportBackupEditor::exportBanks ()
{
  Application::get ().stopWatchDog ();
  writeBackupFileXML ();

  auto timeStamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  auto targetNameWithStamp(std::string("/mnt/usb-stick/") + std::to_string(timeStamp) + c_backupTargetFile);

  auto moveCommand(std::string("mv ") + c_tempBackupFile + " " + targetNameWithStamp);
  SpawnCommandLine cmd(moveCommand);

  Application::get ().runWatchDog ();
  installState (Finished);
}

void ExportBackupEditor::writeBackupFileXML ()
{
  auto &app = Application::get ();
  auto &boled = app.getHWUI ()->getPanelUnit ().getEditPanel ().getBoled ();
  boled.setOverlay (new SplashLayout ());

  auto stream = std::make_shared<FileOutStream> (c_tempBackupFile, true);
  XmlWriter writer (stream);
  auto pm = Application::get ().getPresetManager ();
  PresetManagerSerializer serializer (*pm.get ());
  serializer.write (writer, VersionAttribute::get ());

  boled.resetOverlay();
}

bool ExportBackupEditor::onButton (Buttons i, bool down, ButtonModifiers modifiers)
{
  if (down)
  {
    if (i == Buttons::BUTTON_C && m_state != NotReady)
    {
      installState (Running);
      return true;
    }
    else if (i == Buttons::BUTTON_D)
    {
      diveUp ();
      return true;
    }
  }
  return false;
}

void ExportBackupEditor::setPosition (const Rect &)
{
  ControlWithChildren::setPosition (c_fullRightSidePosition);
}

