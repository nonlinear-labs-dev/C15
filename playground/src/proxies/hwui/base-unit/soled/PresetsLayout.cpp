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
#include <presets/PresetBank.h>
#include <device-settings/Settings.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include "SoledHeader.h"
#include "DirectLoadIndicator.h"

PresetsLayout::PresetsLayout ()
{
  const auto headlineHeight = 16;

  addControl (new SoledHeader ("Preset", Rect (0, 0, 31, headlineHeight + 1)));
  m_number = addControl (new LabelRegular8 ("", Rect (32, 1, 64, headlineHeight - 1)));
  m_name = addControl (new Label ("", Rect (0, headlineHeight + 2, 128, 32 - headlineHeight - 2)));
  m_directLoad = addControl (new DirectLoadIndicator (Rect (96, 0, 32, headlineHeight)));
  addControl(new DottedLine(Rect(31, headlineHeight, 96, 1)));

  auto pm = Application::get ().getPresetManager ();
  pm->onBankSelection (sigc::mem_fun (this, &PresetsLayout::onBankSelected));
  Application::get ().getSettings ()->getSetting<AutoLoadSelectedPreset> ()->onChange (
      sigc::mem_fun (this, &PresetsLayout::onAutoLoadSettingChanged));
  pm->getEditBuffer ()->onModificationStateChanged (sigc::mem_fun (this, &PresetsLayout::onEditBufferModified));
}

PresetsLayout::~PresetsLayout ()
{
}

void PresetsLayout::onBankSelected (shared_ptr<PresetBank> bank)
{
  m_connection.disconnect ();

  if (bank)
    m_connection = bank->onBankChanged (sigc::mem_fun (this, &PresetsLayout::onBankChanged));
}

void PresetsLayout::onBankChanged ()
{
  update ();
}

void PresetsLayout::onAutoLoadSettingChanged (const Setting *s)
{
  update ();
}

void PresetsLayout::onEditBufferModified (bool b)
{
  update ();
}

void PresetsLayout::update ()
{
  updateDirectLoadLabel ();

  if (!updateNameAndNumber ())
  {
    m_number->setText ("---");
    m_name->setText ("---");
  }
}

bool PresetsLayout::updateNameAndNumber ()
{
  auto& app = Application::get ();
  auto pm = app.getPresetManager ();
  auto eb = pm->getEditBuffer ();

  if (auto bank = pm->getSelectedBank ())
  {
    auto presetUUID = getCurrentlySelectedPresetUUID ();

    if (auto preset = bank->getPreset (presetUUID))
    {
      auto bankNumber = pm->calcOrderNumber (bank.get ());
      auto presetPosition = bank->getPresetPosition (presetUUID);
      auto presetNumberString = formatBankAndPresetNumber (bankNumber, presetPosition, eb->isModified ());
      m_number->setText (presetNumberString);
      m_name->setText (preset->getName ());
      return true;
    }
  }
  return false;
}

Glib::ustring PresetsLayout::formatBankAndPresetNumber (size_t bankNumber, size_t presetPosition, bool modified) const
{
  char txt[128];
  const auto modifiedMark = modified ? "*" : "";
  sprintf (txt, "%zu-%03zu%s", bankNumber, presetPosition + 1, modifiedMark);
  return txt;
}

void PresetsLayout::updateDirectLoadLabel ()
{
  auto& app = Application::get ();
  auto pm = app.getPresetManager ();
  auto eb = pm->getEditBuffer ();

  auto currentlyLoadedPresetUUID = eb->getUUIDOfLastLoadedPreset ();
  auto currentlySelectedPresetUUID = getCurrentlySelectedPresetUUID ();

  if (app.getSettings ()->getSetting<AutoLoadSelectedPreset> ()->get ())
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::DirectLoad);
  }
  else if (currentlyLoadedPresetUUID != currentlySelectedPresetUUID || eb->isModified ())
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::Load);
  }
  else
  {
    m_directLoad->setMode(DirectLoadIndicator::Mode::Off);
  }
}

Glib::ustring PresetsLayout::getCurrentlySelectedPresetUUID () const
{
  auto pm = Application::get ().getPresetManager ();

  if (auto bank = pm->getSelectedBank ())
    return bank->getSelectedPreset ();

  return
  {};
}
