#include <Application.h>
#include <proxies/hwui/base-unit/soled/BanksLayout.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <proxies/hwui/controls/DottedLine.h>
#include <proxies/hwui/TextCropper.h>
#include "SoledHeader.h"
#include "DirectLoadIndicator.h"

class ShortenLabel : public Label
{
 public:
  ShortenLabel(const ustring &text, const Rect &pos)
      : Label(text, pos)
  {
  }

 protected:
  StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const override
  {
    return TextCropper::shortenStringIfNeccessary(font, text.text, getPosition().getWidth());
  }
};

BanksLayout::BanksLayout()
    : super(Application::get().getHWUI()->getBaseUnit().getPlayPanel().getSOLED())
{
  const auto headlineHeight = 16;
  addControl(new SoledHeader("Bank", Rect(0, 0, 31, headlineHeight + 1)));

  m_number = addControl(new LabelRegular8("", Rect(32, 1, 64, headlineHeight - 1)));
  m_name = addControl(new ShortenLabel("", Rect(0, headlineHeight + 2, 128, 32 - headlineHeight - 2)));
  m_directLoad = addControl(new DirectLoadIndicator(Rect(96, 0, 32, headlineHeight)));
  addControl(new DottedLine(Rect(31, headlineHeight, 96, 1)));

  Application::get().getPresetManager()->onBankSelection(sigc::mem_fun(this, &BanksLayout::onBankSelected));
  Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->onChange(
      sigc::mem_fun(this, &BanksLayout::onAutoLoadSettingChanged));
}

BanksLayout::~BanksLayout()
{
}

void BanksLayout::onBankSelected(const Uuid &selectedBank)
{
  m_bank = Application::get().getPresetManager()->getSelectedBank();
  m_bankconnection.disconnect();
  if(m_bank)
    m_bankconnection = m_bank->onBankChanged(sigc::mem_fun(this, &BanksLayout::onBankChanged));

  updateFromBank(m_bank);
}

void BanksLayout::updateFromBank(const Bank *bank) const
{
  if(bank)
  {
    auto order = Application::get().getPresetManager()->getBankPosition(bank->getUuid()) + 1;
    auto str = ustring::format(order);
    m_number->setText(str);
    m_name->setText({ bank->getName(true) });
  }
  else
  {
    m_number->setText("---");
    m_name->setText("---");
  }
}

void BanksLayout::onAutoLoadSettingChanged(const Setting *s)
{
  bool on = Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->get();
  m_directLoad->setMode(on ? DirectLoadIndicator::Mode::DirectLoad : DirectLoadIndicator::Mode::Off);
}

void BanksLayout::onBankChanged()
{
  updateFromBank(m_bank);
}
