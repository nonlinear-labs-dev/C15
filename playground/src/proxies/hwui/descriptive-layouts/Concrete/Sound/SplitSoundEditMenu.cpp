#include "SplitSoundEditMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/EditorItem.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/ArrowLeft.h>
#include <proxies/hwui/controls/ArrowRight.h>

class ConvertToSingle : public BasicItem
{

 public:
  ConvertToSingle()
      : BasicItem("Convert to Single")
  {
  }

  void doAction() override
  {
    Application::get().getPresetManager()->getEditBuffer()->setType(Type::Single);
  }
};

class InitSound : public BasicItem
{

 public:
  InitSound()
      : BasicItem("Init")
  {
  }

  void doAction() override
  {
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  }
};

class GenericItem : public BasicItem
{
 public:
  template <class T, class tCB>
  GenericItem(const T& caption, tCB cb)
      : BasicItem(caption)
      , m_cb{ cb }
  {
  }

  void doAction() override
  {
    m_cb();
  }

 protected:
  std::function<void(void)> m_cb;
};

class lal : public LeftAlignedLabel {
public:
  template<class cap>
  lal(cap c, const Rect& r) : LeftAlignedLabel{c, r} {
  }

protected:
  void setBackgroundColor(FrameBuffer &fb) const override {
    fb.setColor(FrameBuffer::C103);
  }
};

class RandomizeOverlay : public ScrollMenuOverlay
{
 public:
  explicit RandomizeOverlay(const Rect& r)
      : ScrollMenuOverlay(r)
  {



    auto label2 = addControl(new lal("Randomize", { 0, 0, 128, 13 }));
    valueLabel = addControl(new Label({ "0.0", 0 }, { 150, 0, 64, 13 }));
    left = addControl(new ArrowLeft({ 128, -1, 10, 13 }));
    right = addControl(new ArrowRight({ 214, -1, 10, 13 }));
    auto setting = Application::get().getSettings()->getSetting<RandomizeAmount>();

    m_connection = setting->onChange([=](const Setting* s) {
      if(auto rand = dynamic_cast<const RandomizeAmount*>(s))
        valueLabel->setText({ rand->getDisplayString(), 0 });
    });
  }

  ~RandomizeOverlay() override
  {
    m_connection.disconnect();
    m_actionConnection.disconnect();
  }

  bool onButton(Buttons i, bool down, ButtonModifiers mod) override
  {
    auto setting = Application::get().getSettings()->getSetting<RandomizeAmount>();

    if(down)
    {
      if(i == Buttons::BUTTON_C)
      {
        setting->incDec(-1, {});
        left->setHighlight(true);
        return true;
      }
      else if(i == Buttons::BUTTON_D)
      {
        setting->incDec(1, {});
        right->setHighlight(true);
        return true;
      }
      else if(i == Buttons::BUTTON_ENTER)
      {
        doRandomize();
        return true;
      }
    }
    else
    {
      if(i == Buttons::BUTTON_C)
        left->setHighlight(false);
      if(i == Buttons::BUTTON_D) {
        right->setHighlight(false);
      }
    }

    return false;
  }

 protected:
  void doRandomize() const
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto scope = eb->getParent()->getUndoScope().startTransaction("Randomize Editbuffer");
    eb->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_HWUI);

    valueLabel->setHighlight(true);

    m_actionConnection = Application::get().getMainContext()->signal_timeout().connect(
        [&] {
          valueLabel->setHighlight(false);
          return false;
        },
        250);
  }

  Label* valueLabel;
  SymbolLabel* left;
  SymbolLabel* right;

  sigc::connection m_connection;
  mutable sigc::connection m_actionConnection;
};

class RandomizeEditor : public EditorItem
{
 public:
  RandomizeEditor()
      : EditorItem("Randomize")
  {
  }

  ScrollMenuOverlay* createEditor() override
  {
    return new RandomizeOverlay(getPosition());
  }

  void doAction() override
  {
  }
};

SplitSoundEditMenu::SplitSoundEditMenu()
    : ScrollMenu()
{
  init();
}

void SplitSoundEditMenu::init()
{
  addItem<GenericItem>("Import Preset", []() {
    Application::get().getHWUI()->setFocusAndMode(UIDetail::SoundSelectPresetForVoiceGroup);
  });
  addItem<ConvertToSingle>();
  addItem<InitSound>();
  addItem<RandomizeEditor>();
  // addItem<MonoModeEditor>();
  //addItem<TransitionTimeEditor>();
  //addItem<TuneReferenceEditor>();
}
