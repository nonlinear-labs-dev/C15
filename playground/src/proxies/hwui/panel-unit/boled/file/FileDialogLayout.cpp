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

FileDialogLayout::FileDialogLayout(tFilterFunction filter, tCallBackFunction cb, std::string header) :
    commitFunction(cb),
    m_header(header),
    crawler("/mnt/usb-stick/", filter, [ = ]()
    {
      auto fl = crawler.copyData();
      fileCount = fl.size();
      fileList->setFileList(fl);
      updateLabels();
    })

{
  fileCount = 0;
  fileList = addControl(new FileListControl());
  headerLabel = addControl(new InvertedLabel(header, Rect(0, 0, 200, 14)));
  fileList->setPosition(Rect(0, 14, 200, 50));
  positionLabel = addControl(new Label("", Rect(200, 0, 56, 14)));
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
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();

  if(down)
  {
    switch(i)
    {
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
        hwui->undoableSetFocusAndMode( { UIFocus::Banks, UIMode::Select });
        return true;

      case Buttons::BUTTON_INFO:
        if(fileCount > 0)
          overlayInfo();
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
    positionLabel->setText("[" + std::to_string(fileList->getSelectedIndex() + 1) + "/" + std::to_string(fileCount) + "]");
  else
    positionLabel->setText("[0/0]");
}

std::experimental::filesystem::directory_entry FileDialogLayout::getSelectedFile()
{
  return fileList->getSelection();
}

bool FileDialogLayout::redraw(FrameBuffer &fb)
{
  DFBLayout::redraw(fb);
  fb.setColor(FrameBuffer::Colors::C128);
  Rect r(0, 0, 200, 64);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
  return true;
}

