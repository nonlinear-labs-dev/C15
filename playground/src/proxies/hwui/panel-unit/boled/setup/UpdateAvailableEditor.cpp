#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableEditor.h>

UpdateAvailableEditor::UpdateAvailableEditor()
    : super("To apply the update turn off the C15 and restart it.")
{
  setPosition(Rect(128, 0, 128, 64));
}

UpdateAvailableEditor::~UpdateAvailableEditor()
{
}
