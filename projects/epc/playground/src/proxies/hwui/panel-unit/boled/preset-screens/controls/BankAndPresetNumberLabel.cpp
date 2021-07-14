#include "BankAndPresetNumberLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <proxies/hwui/HWUI.h>
#include <iomanip>
#include <locale>
#include <sstream>

BankAndPresetNumberLabel::BankAndPresetNumberLabel(const Rect &pos)
    : super(pos)
{
  auto pm = Application::get().getPresetManager();
  auto eb = pm->getEditBuffer();
  pm->onNumBanksChanged(sigc::hide<0>(mem_fun(this, &BankAndPresetNumberLabel::onEditBufferChanged)));
  eb->onChange(sigc::mem_fun(this, &BankAndPresetNumberLabel::onEditBufferChanged));
  eb->onModificationStateChanged(sigc::hide<0>(mem_fun(this, &BankAndPresetNumberLabel::onEditBufferChanged)));
}

void BankAndPresetNumberLabel::onEditBufferChanged()
{
  setDirty();
}

bool BankAndPresetNumberLabel::redraw(FrameBuffer &fb)
{
  auto pm = Application::get().getPresetManager();
  auto hwui = Application::get().getHWUI();
  auto uuid = pm->getEditBuffer()->getUUIDOfLastLoadedPreset();
  Glib::ustring text = "";

  if(auto bank = pm->findBankWithPreset(uuid))
  {
    auto bankPos = pm->getBankPosition(bank->getUuid()) + 1;
    auto presetPos = bank->getPresetPosition(uuid) + 1;

    std::ostringstream presetPosStr;
    presetPosStr.width(3);
    presetPosStr.fill('0');
    presetPosStr << presetPos;
    text = nltools::string::concat(bankPos, "-", presetPosStr.str());
  }
  else if(uuid == Uuid::converted())
  {
    text = "Converted";
  }
  else if(uuid == Uuid::init())
  {
    text = "Init";
  }
  else if(uuid == Uuid::restored())
  {
    text = "Restored";
  }
  else
  {
    text = "";
  }

  if(pm->getEditBuffer()->findAnyParameterChanged() && !text.empty())
    text += "*";

  setText(text);

  Label::redraw(fb);
  return true;
}
