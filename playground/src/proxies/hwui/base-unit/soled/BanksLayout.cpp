#include <Application.h>
#include <proxies/hwui/base-unit/soled/BanksLayout.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <presets/PresetManager.h>
#include <presets/PresetBank.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <proxies/hwui/controls/DottedLine.h>
#include "SoledHeader.h"
#include "DirectLoadIndicator.h"

BanksLayout::BanksLayout ()
{
  const auto headlineHeight = 16;
  addControl (new SoledHeader ("Bank", Rect (0, 0, 31, headlineHeight + 1)));

  m_number = addControl(new LabelRegular8("", Rect(32, 1, 64, headlineHeight - 1)));
  m_name = addControl(new Label("", Rect(0, headlineHeight + 2, 128, 32 - headlineHeight - 2)));
  m_directLoad = addControl (new DirectLoadIndicator (Rect (96, 0, 32, headlineHeight)));
  addControl(new DottedLine(Rect(31, headlineHeight, 96, 1)));

  Application::get().getPresetManager()->onBankSelection (sigc::mem_fun(this, &BanksLayout::onBankSelected));
  Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->onChange(sigc::mem_fun(this, &BanksLayout::onAutoLoadSettingChanged));
}

BanksLayout::~BanksLayout ()
{
}

void BanksLayout::onBankSelected(shared_ptr<PresetBank> bank)
{
  if(bank)
  {
    auto order = Application::get().getPresetManager()->calcOrderNumber (bank.get());
    auto numBanks = Application::get().getPresetManager()->getNumBanks();
    auto str = Glib::ustring::format(order);
    m_number->setText(str);
    m_name->setText(bank->getName(true));
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
