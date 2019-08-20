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

class cal : public LeftAlignedLabel
{
 public:
  template <class cap>
  cal(cap c, const Rect& r)
      : LeftAlignedLabel{ c, r }
  {
  }

  Font::Justification getJustification() const override {
    return Font::Justification::Center;
  }

protected:
  void setBackgroundColor(FrameBuffer& fb) const override
  {
    fb.setColor(FrameBuffer::C103);
  }
};

class ChangeValueAndCommitOverlay : public ScrollMenuOverlay
{
 public:
  template <class tCaption>
  explicit ChangeValueAndCommitOverlay(const tCaption& caption, const Rect& r)
      : ScrollMenuOverlay(r)
  {
    constexpr const auto cArrowWidth = 10;
    constexpr const auto cDoubleArrowWidth = cArrowWidth * 2;


    auto full = r;
    full.setLeft(full.getLeft() + cArrowWidth);
    full.setWidth(r.getWidth() - cDoubleArrowWidth);
    auto left = r;
    left.setLeft(full.getLeft() - cArrowWidth);
    left.setWidth(cArrowWidth);
    auto right = left;
    right.setLeft(full.getRight());

    m_valueLabel = addControl(new Label(caption, full));
    m_leftArrow = addControl(new ArrowLeft(left));
    m_rightArrow = addControl(new ArrowRight(right));
  }

  bool onButton(Buttons i, bool down, ButtonModifiers mod) override
  {
    if(down)
    {
      if(i == Buttons::BUTTON_C)
      {
        dec();
        return true;
      }
      if(i == Buttons::BUTTON_D)
      {
        inc();
        return true;
      }
      if(i == Buttons::BUTTON_ENTER)
      {
        commit();
        return true;
      }
    }
    return false;
  }

  virtual void inc() = 0;
  virtual void dec() = 0;
  virtual void commit() = 0;

 protected:
  Label* m_valueLabel;
  SymbolLabel* m_leftArrow;
  SymbolLabel* m_rightArrow;
};

class RandomizeOverlay : public ChangeValueAndCommitOverlay
{
 public:
  explicit RandomizeOverlay(const Rect& r)
      : ChangeValueAndCommitOverlay("HALLO HALLO", r)
  {
    m_settingConnection = getSetting()->onChange([=](const Setting* s) {
      if(auto rand = dynamic_cast<const RandomizeAmount*>(s))
        m_valueLabel->setText(rand->getDisplayString());
    });
  }

  bool redraw(FrameBuffer &fb) override {
    auto ret = ChangeValueAndCommitOverlay::redraw(fb);
    ret |= m_valueLabel->redraw(fb);
    ret |= m_leftArrow->redraw(fb);
    ret |= m_rightArrow->redraw(fb);
    return ret;
  }

public:
  ~RandomizeOverlay() override
  {
    m_settingConnection.disconnect();
  }

  static RandomizeAmount* getSetting()
  {
    return Application::get().getSettings()->getSetting<RandomizeAmount>().get();
  }

  void inc() override
  {
    getSetting()->incDec(1, {});
  }

  void dec() override
  {
    getSetting()->incDec(-1, {});
  }

  void commit() override
  {
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Randomize");
    Application::get().getPresetManager()->getEditBuffer()->undoableRandomize(scope->getTransaction(),
                                                                              Initiator::EXPLICIT_HWUI);
  }

 protected:
  sigc::connection m_settingConnection;
};

class RandomizeEditor : public EditorItem
{
 public:
  RandomizeEditor()
      : EditorItem("Randomize")
  {
    auto rightHalf = getPosition();
    rightHalf.setWidth(rightHalf.getWidth() / 2);
    rightHalf.setLeft(rightHalf.getWidth());
    auto label = addControl(new Label("Value", rightHalf));

    m_settingConnection
        = Application::get().getSettings()->getSetting<RandomizeAmount>()->onChange([=](const Setting* s) {
            if(auto rand = dynamic_cast<const RandomizeAmount*>(s))
              label->setText(rand->getDisplayString());
          });
  }

  ~RandomizeEditor() override
  {
    m_settingConnection.disconnect();
  }

  ScrollMenuOverlay* createEditor() override
  {
    auto half = getPosition();
    half.setWidth(half.getWidth() / 2);
    half.setLeft(half.getWidth());

    auto fullRight = Rect{ 128, 11, 127, 52 };

    return new RandomizeOverlay(half);
  }

  void doAction() override
  {
  }

 protected:
  sigc::connection m_settingConnection;
};

SplitSoundEditMenu::SplitSoundEditMenu(const Rect& r)
    : ScrollMenu(r)
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
