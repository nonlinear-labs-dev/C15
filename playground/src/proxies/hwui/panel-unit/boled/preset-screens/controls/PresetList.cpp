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
Preset *PresetList::getPresetAtSelected()
{
  return Application::get().getPresetManager()->getSelectedBank()->getSelectedPreset();
}

GenericPresetList::GenericPresetList(const Point &p)
    : PresetList({ p.getX(), p.getY(), 128, 50 }, true)
{
  sanitizePresetPtr();
}

void GenericPresetList::incBankSelection()
{
  sanitizePresetPtr();
  auto bank = (Bank*)m_selectedPreset->getParent();
  auto pm = (PresetManager*)bank->getParent();
  auto nextBank = pm->getBankAt(pm->getBankPosition(bank->getUuid()) + 1);
  if(nextBank)
    m_selectedPreset = nextBank->getPresetAt(0);
}
void GenericPresetList::decBankSelection()
{
  sanitizePresetPtr();
  auto bank = (Bank*)m_selectedPreset->getParent();
  auto pm = (PresetManager*)bank->getParent();
  auto nextBank = pm->getBankAt(pm->getBankPosition(bank->getUuid()) - 1);
  if(nextBank)
    m_selectedPreset = nextBank->getPresetAt(0);
}
void GenericPresetList::incPresetSelection()
{
  sanitizePresetPtr();
  auto bank = (Bank*)m_selectedPreset->getParent();
  m_selectedPreset = bank->getPresetAt(bank->getPresetPosition(m_selectedPreset) + 1);
}

void GenericPresetList::decPresetSelection()
{
  sanitizePresetPtr();
  auto bank = (Bank*)m_selectedPreset->getParent();
  m_selectedPreset = bank->getPresetAt(bank->getPresetPosition(m_selectedPreset) - 1);
}
bool GenericPresetList::redraw(FrameBuffer &fb)
{
  sanitizePresetPtr();
  const Rect &r = getPosition();
  fb.setColor(FrameBuffer::Colors::C103);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

  drawPresets(fb, m_selectedPreset);

  return true;
}

void GenericPresetList::drawPresets(FrameBuffer &fb, Preset *middle)
{
  if(middle == nullptr)
    return;

  auto pos = getPosition();
  auto third = pos.getHeight() / 3;
  auto bank = dynamic_cast<Bank*>(middle->getParent());
  auto currentPos = bank->getPresetPosition(middle);
  auto prev = bank->getPresetAt(currentPos - 1);
  auto next = bank->getPresetAt(currentPos + 1);

  auto createAndDrawControl = [&](Preset* p, int index) {
    if(p == nullptr)
      return;

    auto c = addControl(new LeftAlignedLabel({p->getName(), 0}, {getPosition().getLeft(), index * third, pos.getWidth(), third}));
    c->setFontColor(FrameBuffer::C179);
    auto rect = c->getPosition();
    fb.setColor(FrameBuffer::C103);
    fb.fillRect(rect.getMargined(0, 2));
    c->redraw(fb);
    remove(c);
  };

  createAndDrawControl(middle, 1);
  createAndDrawControl(prev, 0);
  createAndDrawControl(next, 2);


}
void GenericPresetList::sanitizePresetPtr()
{
  if(m_selectedPreset == nullptr)
    m_selectedPreset = Application::get().getPresetManager()->getSelectedBank()->getSelectedPreset();
}

PresetListVGSelect::PresetListVGSelect(const Point &p)
    : GenericPresetList(p), blocker(Application::get().getPresetManager()->getAutoLoadBlocker())
{
}
void PresetListVGSelect::action()
{
  Application::get().getPresetManager()->getEditBuffer()->loadCurrentVG(getPresetAtSelected());
}
