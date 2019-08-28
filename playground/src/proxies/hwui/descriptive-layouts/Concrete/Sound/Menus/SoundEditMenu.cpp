#include "SoundEditMenu.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>

void SoundEditMenu::onActionPerformed() {
  Application::get().getHWUI()->setFocusAndMode({UIFocus::Sound, UIMode::Select, UIDetail::Init});
}

SoundEditMenu::SoundEditMenu(const Rect &r) : ScrollMenu(r) {

}
