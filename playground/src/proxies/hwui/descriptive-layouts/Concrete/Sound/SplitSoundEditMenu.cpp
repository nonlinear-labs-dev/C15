#include "SplitSoundEditMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>

class ConvertToSingle : public BasicItem {

public:
  ConvertToSingle() : BasicItem("Convert to Single")
  {
  }

  void doAction() override {
    Application::get().getPresetManager()->getEditBuffer()->setType(Type::Single);
  }
};

class InitSound : public BasicItem {

public:
  InitSound() : BasicItem("Init")
  {
  }

  void doAction() override {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  }
};

class GenericItem : public BasicItem {
public:
  template<class T, class tCB>
  GenericItem(const T& caption, tCB cb)
    : BasicItem(caption)
    , m_cb{cb}
  {
  }

  void doAction() override {
    m_cb();
  }

protected:
  std::function<void(void)> m_cb;
};

SplitSoundEditMenu::SplitSoundEditMenu() : ScrollMenu() {
  init();
}

void SplitSoundEditMenu::init() {
  addItem<ConvertToSingle>();
  addItem<InitSound>();
  addItem<GenericItem>("Import Preset", [](){
    Application::get().getHWUI()->setFocusAndMode(UIDetail::SoundSelectPresetForVoiceGroup);
  });
  addItem<ConvertToSingle>();
}
