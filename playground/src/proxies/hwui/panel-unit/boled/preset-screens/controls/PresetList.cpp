#include <Application.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <memory>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include "presets/Preset.h"

PresetList::PresetList(const Rect &pos, bool showBankArrows)
    : super(pos, showBankArrows)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetList::onBankSelectionChanged));
  Application::get().getPresetManager()->onRestoreHappened(mem_fun(this, &PresetList::onBankChanged));
  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      mem_fun(this, &PresetList::onEditBufferChanged));
}

PresetList::~PresetList() = default;

void PresetList::onBankSelectionChanged(const Uuid &selectedBank)
{
  m_bankChangedConnection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    m_bankChangedConnection = bank->onBankChanged(mem_fun(this, &PresetList::onBankChanged));
  }
  else
  {
    onBankChanged();
  }
}

void PresetList::onEditBufferChanged()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  if(m_uuidOfLastLoadedPreset != eb->getUUIDOfLastLoadedPreset())
  {
    m_uuidOfLastLoadedPreset = eb->getUUIDOfLastLoadedPreset();
    onBankChanged();
  }
}

void PresetList::onBankChanged()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    m_header->setup(bank);

    if(auto p = bank->getSelectedPreset())
      m_content->setup(bank, bank->getPresetPosition(p));
    else
      m_content->setup(bank, size_t(-1));
  }
  else
  {
    m_header->setup(nullptr);
    m_content->setup(nullptr, size_t(-1));
  }
}

bool PresetList::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
    auto pm = Application::get().getPresetManager();

    switch(i)
    {
      case Buttons::BUTTON_B:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->selectPreviousPreset();
        }
        else
        {
          pm->selectPreviousBank();
        }
        return true;

      case Buttons::BUTTON_C:
        if(focusAndMode.focus == UIFocus::Banks)
        {
          if(auto bank = pm->getSelectedBank())
            bank->selectNextPreset();
        }
        else
        {
          pm->selectNextBank();
        }
        return true;
    }
  }

  return false;
}

void PresetList::onRotary(int inc, ButtonModifiers modifiers)
{
  auto focusAndMode = Application::get().getHWUI()->getFocusAndMode();
  auto pm = Application::get().getPresetManager();

  if(focusAndMode.focus == UIFocus::Banks)
  {
    auto scope = pm->getUndoScope().startTransaction("Select Bank");

    if(modifiers[SHIFT] && pm->getNumBanks() > 0)
    {
      if(inc < 0)
        pm->selectBank(scope->getTransaction(), pm->getBanks().front()->getUuid());
      else
        pm->selectBank(scope->getTransaction(), pm->getBanks().back()->getUuid());
    }
    else
    {
      while(inc < 0)
      {
        pm->selectPreviousBank(scope->getTransaction());
        inc++;
      }

      while(inc > 0)
      {
        pm->selectNextBank(scope->getTransaction());
        inc--;
      }
    }
  }
  else if(auto bank = pm->getSelectedBank())
  {
    auto scope = pm->getUndoScope().startTransaction("Select Preset");
    while(inc < 0)
    {
      bank->selectPreviousPreset(scope->getTransaction());
      inc++;
    }

    while(inc > 0)
    {
      bank->selectNextPreset(scope->getTransaction());
      inc--;
    }
  }
}

std::pair<size_t, size_t> PresetList::getSelectedPosition() const
{
  auto pm = Application::get().getPresetManager();

  if(auto b = pm->getSelectedBank())
  {
    auto bankPos = pm->getBankPosition(b->getUuid());
    auto presetPos = b->getPresetPosition(b->getSelectedPreset());
    return std::make_pair(bankPos, presetPos);
  }
  return { -1, -1 };
}

GenericPresetList::GenericPresetList(const Point &p)
    : ControlWithChildren({ p.getX(), p.getY(), 128, 50 })
{
  sanitizePresetPtr();

  m_restoreHappened = Application::get().getPresetManager()->onRestoreHappened([&]() { signalChanged(); });
  m_numBanks = Application::get().getPresetManager()->onNumBanksChanged([&](size_t numB) { signalChanged(); });

  signalChanged();
}

GenericPresetList::~GenericPresetList()
{
  m_restoreHappened.disconnect();
  m_numBanks.disconnect();
}

void GenericPresetList::incBankSelection()
{
  if(!sanitizePresetPtr())
    return;

  auto bank = (Bank *) m_selectedPreset->getParent();
  auto pm = (PresetManager *) bank->getParent();
  auto nextBank = pm->getBankAt(pm->getBankPosition(bank->getUuid()) + 1);
  if(nextBank)
    m_selectedPreset = nextBank->getPresetAt(0);

  signalChanged();
}

void GenericPresetList::decBankSelection()
{
  if(!sanitizePresetPtr())
    return;
  auto bank = (Bank *) m_selectedPreset->getParent();
  auto pm = (PresetManager *) bank->getParent();
  auto nextBank = pm->getBankAt(pm->getBankPosition(bank->getUuid()) - 1);
  if(nextBank)
    m_selectedPreset = nextBank->getPresetAt(0);

  signalChanged();
}

void GenericPresetList::incPresetSelection()
{
  if(!sanitizePresetPtr())
    return;
  auto bank = (Bank *) m_selectedPreset->getParent();
  if(auto candidate = bank->getPresetAt(bank->getPresetPosition(m_selectedPreset) + 1))
    m_selectedPreset = candidate;

  signalChanged();
}

void GenericPresetList::decPresetSelection()
{
  auto bank = (Bank *) m_selectedPreset->getParent();
  if(auto candidate = bank->getPresetAt(bank->getPresetPosition(m_selectedPreset) - 1))
    m_selectedPreset = candidate;

  signalChanged();
}

Preset *GenericPresetList::getPresetAtSelected() const
{
  return m_selectedPreset;
}

bool GenericPresetList::redraw(FrameBuffer &fb)
{

  const Rect &r = getPosition();
  fb.setColor(FrameBuffer::Colors::C103);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

  if(!sanitizePresetPtr())
    return true;

  drawPresets(fb, m_selectedPreset);

  return true;
}

sigc::connection GenericPresetList::onChange(sigc::slot<void(GenericPresetList *)> pl)
{
  return m_signalChanged.connect(pl);
}

void GenericPresetList::signalChanged()
{
  m_signalChanged.emit(this);
}

void GenericPresetList::drawPresets(FrameBuffer &fb, Preset *middle)
{
  if(middle == nullptr)
    return;

  auto pos = getPosition();
  auto third = pos.getHeight() / 3;
  auto pBank = dynamic_cast<Bank *>(middle->getParent());
  auto currentPos = pBank->getPresetPosition(middle);
  auto prev = pBank->getPresetAt(currentPos - 1);
  auto next = pBank->getPresetAt(currentPos + 1);

  auto createAndDrawControl = [&](Preset *p, int index, bool hightlight) {
    if(p == nullptr)
      return;

    auto basePos = getPosition();

    auto bank = dynamic_cast<Bank *>(p->getParent());
    auto presetNum = bank->getPresetPosition(p);

    auto numPos = Rect{ basePos.getLeft() + 10, basePos.getTop() + index * third, 20, third };
    auto namePos = Rect{ basePos.getLeft() + 32, basePos.getTop() + index * third, basePos.getWidth() - 32, third };
    auto number = addControl(new LeftAlignedLabel({ std::to_string(presetNum), 0 }, numPos));
    auto name = addControl(new LeftAlignedLabel({ p->getName(), 0 }, namePos));


    if(hightlight)
    {
      auto rect = getPosition();
      rect.setTop(rect.getTop() + index * third);
      rect.setHeight(third);
      rect.addMargin(2, 2, 2, 2);
      fb.setColor(FrameBuffer::C103);
      fb.fillRect(rect);

      fb.setColor(FrameBuffer::C204);
      fb.drawRect({ getPosition().getLeft(), basePos.getTop() + index * third, basePos.getWidth(), third });
    }

    number->setFontColor(hightlight ? FrameBuffer::C255 : FrameBuffer::C179);
    number->redraw(fb);

    name->setFontColor(hightlight ? FrameBuffer::C255 : FrameBuffer::C179);
    name->redraw(fb);

    remove(number);
    remove(name);
  };

  createAndDrawControl(middle, 1, true);
  createAndDrawControl(prev, 0, false);
  createAndDrawControl(next, 2, false);
}
bool GenericPresetList::sanitizePresetPtr()
{
  if(m_selectedPreset == nullptr)
    m_selectedPreset = Application::get().getPresetManager()->getSelectedBank()->getSelectedPreset();

  return m_selectedPreset != nullptr;
}

PresetListVGSelect::PresetListVGSelect(const Point &p)
    : GenericPresetList(p)
{
}

void PresetListVGSelect::action()
{
  Application::get().getPresetManager()->getEditBuffer()->loadCurrentVG(getPresetAtSelected());
  Application::get().getHWUI()->setFocusAndMode(UIDetail::Init);
}
