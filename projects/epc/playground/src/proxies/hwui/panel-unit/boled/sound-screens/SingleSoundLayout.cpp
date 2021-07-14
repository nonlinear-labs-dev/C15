#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <device-settings/Settings.h>
#include <device-settings/TransitionTime.h>
#include <http/UndoScope.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>
#include <playground.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/Caption.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/RandomizeAmountSlider.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenuLegacy.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/TransitionTimeLabel.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/TransitionTimeSlider.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/SingleSoundLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/UsageMode.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LockedIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/AnyParameterLockedIndicator.h>

SingleSoundLayout::SingleSoundLayout(FocusAndMode focusAndMode)
    : Layout(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
{
  addControl(new Caption("Deprecated", Rect(0, 0, 64, 13)));
}