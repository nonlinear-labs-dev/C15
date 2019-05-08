#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/PresetBank.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetInfoCaption.h>

PresetInfoCaption::PresetInfoCaption(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetInfoCaption::onBankSelection));
}

PresetInfoCaption::~PresetInfoCaption()
{
}

void PresetInfoCaption::onBankSelection(PresetManager::tBankPtr bank)
{
  m_bankConnection.disconnect();

  if(bank)
  {
    m_bankConnection = bank->onBankChanged(mem_fun(this, &PresetInfoCaption::onBankChanged));
  }
}

void PresetInfoCaption::onBankChanged()
{
  auto pm = Application::get().getPresetManager();
  if(auto bank = pm->getSelectedBank())
  {
    auto bankNumber = pm->calcOrderNumber(bank.get());
    auto selectedPresetUUID = bank->getSelectedPreset();

    if(auto selecteddPreset = bank->getPreset(selectedPresetUUID))
    {
      auto presetNumber = bank->getPresetPosition(selectedPresetUUID) + 1;

      ostringstream presetPosStr;
      presetPosStr.width(3);
      presetPosStr.fill('0');
      presetPosStr << presetNumber;

      auto text = UNDO::StringTools::buildString(bankNumber, "-", presetPosStr.str());
      setText(text);
      return;
    }
  }

  setText("");
}

bool PresetInfoCaption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void PresetInfoCaption::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

shared_ptr<Font> PresetInfoCaption::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int PresetInfoCaption::getFontHeight() const
{
  return 8;
}
