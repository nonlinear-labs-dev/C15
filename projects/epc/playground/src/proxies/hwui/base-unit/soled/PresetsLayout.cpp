#include <Application.h>
#include <proxies/hwui/base-unit/soled/PresetsLayout.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <proxies/hwui/controls/DottedLine.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <proxies/hwui/TextCropper.h>
#include "SoledHeader.h"
#include "DirectLoadIndicator.h"
#include <presets/Preset.h>

class ShortenLabel : public Label
{
 public:
  ShortenLabel(const Glib::ustring &text, const Rect &pos)
      : Label(StringAndSuffix { text }, pos)
  {
  }

 protected:
  StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                            const StringAndSuffix &text) const override
  {
    return StringAndSuffix { TextCropper::shortenStringIfNeccessary(font, text.text, getPosition().getWidth()) };
  }
};

PresetsLayout::PresetsLayout()
    : super(Application::get().getHWUI()->getBaseUnit().getPlayPanel().getSOLED())
{
  const auto headlineHeight = 16;

  addControl(new SoledHeader("Preset", Rect(0, 0, 31, headlineHeight + 1)));
  m_number = addControl(new LabelRegular8("", Rect(32, 1, 64, headlineHeight - 1)));
  m_name = addControl(new ShortenLabel("", Rect(0, headlineHeight + 2, 120, 32 - headlineHeight - 2)));
  m_type = addControl(new ShortenLabel("", Rect(120, headlineHeight, 8, 32 - headlineHeight - 2)));
  m_directLoad = addControl(new DirectLoadIndicator(Rect(96, 0, 32, headlineHeight)));
  addControl(new DottedLine(Rect(31, headlineHeight, 96, 1)));

  auto pm = Application::get().getPresetManager();

  pm->getEditBuffer()->onPresetLoaded(sigc::mem_fun(this, &PresetsLayout::onPresetLoaded));
  pm->getEditBuffer()->onModificationStateChanged(sigc::mem_fun(this, &PresetsLayout::onEditBufferModified));

  pm->onBankSelection(sigc::mem_fun(this, &PresetsLayout::onBankSelected));

  Application::get().getSettings()->getSetting<DirectLoadSetting>()->onChange(
      sigc::mem_fun(this, &PresetsLayout::onAutoLoadSettingChanged));
}

PresetsLayout::~PresetsLayout() = default;

void PresetsLayout::onBankSelected(const Uuid &selectedBank)
{
  m_connection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
    m_connection = bank->onBankChanged(sigc::mem_fun(this, &PresetsLayout::onBankChanged));
}

void PresetsLayout::onBankChanged()
{
  update();
}

void PresetsLayout::onPresetLoaded()
{
  update();
}

void PresetsLayout::onAutoLoadSettingChanged(const Setting *s)
{
  update();
}

void PresetsLayout::onEditBufferModified(bool b)
{
  update();
}

void PresetsLayout::update()
{
  updateDirectLoadLabel();

  if(!updateNameAndNumber())
  {
    m_number->setText(StringAndSuffix { "---" });
    m_name->setText(StringAndSuffix { "---" });
  }
}

bool PresetsLayout::updateNameAndNumber()
{
  auto &app = Application::get();
  auto pm = app.getPresetManager();
  auto eb = pm->getEditBuffer();

  if(auto bank = pm->getSelectedBank())
  {
    auto presetUUID = getCurrentlySelectedPresetUUID();

    if(auto preset = bank->findPreset(presetUUID))
    {
      auto bankNumber = pm->getBankPosition(bank->getUuid()) + 1;
      auto presetPosition = bank->getPresetPosition(presetUUID);
      auto modified = eb->isModified() && eb->getUUIDOfLastLoadedPreset() == presetUUID;
      auto presetNumberString = formatBankAndPresetNumber(bankNumber, presetPosition, modified);
      m_number->setText(presetNumberString);
      m_name->setText(preset->getDisplayNameWithSuffixes(false));
      m_type->setText(preset->getTypeUnicode());
      return true;
    }
  }
  return false;
}

Glib::ustring PresetsLayout::formatBankAndPresetNumber(size_t bankNumber, size_t presetPosition, bool modified) const
{
  char txt[128];
  const auto modifiedMark = modified ? "*" : "";
  sprintf(txt, "%zu-%03zu%s", bankNumber, presetPosition + 1, modifiedMark);
  return txt;
}

void PresetsLayout::updateDirectLoadLabel()
{
  auto &app = Application::get();
  auto pm = app.getPresetManager();
  auto eb = pm->getEditBuffer();

  auto currentlyLoadedPresetUUID = eb->getUUIDOfLastLoadedPreset();
  auto currentlySelectedPresetUUID = getCurrentlySelectedPresetUUID();

  if(app.getSettings()->getSetting<DirectLoadSetting>()->get())
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::DirectLoad);
  }
  else if(currentlyLoadedPresetUUID != currentlySelectedPresetUUID || eb->isModified())
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::Load);
  }
  else
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::Off);
  }
}

Uuid PresetsLayout::getCurrentlySelectedPresetUUID() const
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
    return bank->getSelectedPresetUuid();

  return Uuid::none();
}
