#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/PresetBank.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankInfoCaption.h>

BankInfoCaption::BankInfoCaption(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &BankInfoCaption::onBankSelection));
}

BankInfoCaption::~BankInfoCaption()
{
}

void BankInfoCaption::onBankSelection(PresetManager::tBankPtr bank)
{
  m_bankConnection.disconnect();

  if(bank)
  {
    m_bankConnection = bank->onBankChanged(mem_fun(this, &BankInfoCaption::onBankChanged));
  }
}

void BankInfoCaption::onBankChanged()
{
  auto pm = Application::get().getPresetManager();
  if(auto bank = pm->getSelectedBank())
  {
    auto bankNumber = pm->calcOrderNumber(bank.get());
    auto text = std::to_string(bankNumber);
    setText(text);
    return;
  }
}

bool BankInfoCaption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void BankInfoCaption::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

shared_ptr<Font> BankInfoCaption::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int BankInfoCaption::getFontHeight() const
{
  return 8;
}
