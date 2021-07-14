#include <utility>

#include <proxies/hwui/panel-unit/boled/file/FileDialogLayout.h>
#include <proxies/hwui/panel-unit/boled/file/FileDialogInfoLayout.h>
#include <device-settings/DebugLevel.h>
#include <proxies/hwui/controls/Point.h>

#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/PanelUnitParameterEditMode.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/PanelUnitPresetMode.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/controls/Button.h>

FileDialogLayout::FileDialogLayout(tFilterFunction filter, tCallBackFunction cb, const std::string& header)
    : Layout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
    , commitFunction(std::move(cb))
    , m_header(header)
    , crawler("/mnt/usb-stick/", std::move(filter), [=]() {
      auto fl = crawler.copyData();
      fileCount = fl.size();
      fileList->setFileList(fl);
      updateLabels();
    })

{
  fileCount = 0;
  addControl(new Button("Cancel", Buttons::BUTTON_A));
  addControl(new Button("Select", Buttons::BUTTON_D));
  fileList = addControl(new FileListControl(Rect(0, 14, 256, 36)));
  addControl(new InvertedLabel(header, Rect(0, 0, 256, 14)));
  positionLabel = addControl(new InvertedLabel("", Rect(200, 0, 56, 14)));
  updateLabels();
  crawler.start();
}

FileDialogLayout::~FileDialogLayout()
{
  crawler.killMe();
}

bool FileDialogLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  auto hwui = Application::get().getHWUI();

  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_D:
      case Buttons::BUTTON_ENTER:
        try
        {
          commitFunction(getSelectedFile());
        }
        catch(...)
        {
          DebugLevel::error(__FILE__, __LINE__);
        }
        return true;
      case Buttons::BUTTON_PRESET:
        hwui->undoableSetFocusAndMode({ UIFocus::Banks, UIMode::Select });
        return true;
      case Buttons::BUTTON_INC:
        fileList->changeSelection(1);
        updateLabels();
        return true;
      case Buttons::BUTTON_DEC:
        fileList->changeSelection(-1);
        updateLabels();
        return true;

      case Buttons::BUTTON_INFO:
        if(fileCount > 0)
          overlayInfo();
        return true;
      case Buttons::BUTTON_A:
        getOLEDProxy().resetOverlay();
        return true;
    }
  }
  return Application::get().getHWUI()->getPanelUnit().getUsageMode()->onButtonPressed(i, modifiers, down);
}

void FileDialogLayout::overlayInfo()
{
  auto HWUI = Application::get().getHWUI();
  auto& panel = HWUI->getPanelUnit();
  auto& bol = panel.getEditPanel().getBoled();
  bol.setOverlay(new FileDialogInfoLayout(getSelectedFile(), m_header));
}

bool FileDialogLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  fileList->changeSelection(inc);
  updateLabels();
  return true;
}

void FileDialogLayout::updateLabels()
{
  if(fileCount != 0)
    positionLabel->setText(
        StringAndSuffix { "[" + to_string(fileList->getSelectedIndex() + 1) + "/" + to_string(fileCount) + "]" });
  else
    positionLabel->setText(StringAndSuffix { "[0/0]" });
}

std::filesystem::directory_entry FileDialogLayout::getSelectedFile()
{
  return fileList->getSelection();
}
