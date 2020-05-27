#include <Application.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetInfoContent.h>
#include <sstream>
#include <tools/TimeTools.h>
#include <proxies/hwui/panel-unit/boled/info/MultiLineInfoContent.h>
#include <proxies/hwui/panel-unit/boled/info/InfoField.h>
#include <presets/PresetParameter.h>

PresetInfoContent::PresetInfoContent()
{
  addInfoField("name", "Name", new MultiLineInfoContent());
  addInfoField("type", "Type");
  addInfoField("comment", "Comment", new MultiLineInfoContent());
  addInfoField("lastchange", "Last Change");
  addInfoField("devicename", "Device Name");
  addInfoField("uiversion", "UI Version");

  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetInfoContent::onBankChanged));
}

PresetInfoContent::~PresetInfoContent() = default;

void PresetInfoContent::onBankChanged(const Uuid &)
{
  m_bankConnection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
    m_bankConnection = bank->onBankChanged(mem_fun(this, &PresetInfoContent::onPresetSelectionChanged));
}

void PresetInfoContent::onPresetSelectionChanged()
{
  if(auto preset = getCurrentPreset())
  {
    connectToPreset(preset);
  }
  else if(fillDefaults())
  {
    fixLayout();
  }
}

void PresetInfoContent::connectToPreset(Preset *preset)
{
  m_presetConnection.disconnect();
  m_presetConnection = preset->onChanged(mem_fun(this, &PresetInfoContent::onPresetChanged));
}

void PresetInfoContent::onPresetChanged()
{
  if(getCurrentPreset())
  {
    updateContent();
  }
}

Preset *PresetInfoContent::getCurrentPreset()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
    return bank->getSelectedPreset();

  return nullptr;
}

void PresetInfoContent::fillContents()
{
  fillFromPreset(getCurrentPreset());
}

void PresetInfoContent::fillFromPreset(const Preset *preset)
{
  infoFields["name"]->setInfo(preset->getName(), FrameBufferColors::C128);
  infoFields["type"]->setInfo(createPresetTypeString(preset), FrameBufferColors::C128);
  infoFields["comment"]->setInfo(preset->getAttribute("Comment", "---"), FrameBufferColors::C128);
  infoFields["lastchange"]->setInfo(TimeTools::getDisplayStringFromIso(preset->getAttribute("StoreTime", "---")));
  infoFields["devicename"]->setInfo(preset->getAttribute("DeviceName", "---"));
  infoFields["uiversion"]->setInfo(preset->getAttribute("SoftwareVersion", "---"));
}

bool PresetInfoContent::fillDefaults()
{
  infoFields["name"]->setInfo("---", FrameBufferColors::C128);
  infoFields["type"]->setInfo("---", FrameBufferColors::C128);
  infoFields["comment"]->setInfo("---", FrameBufferColors::C128);
  infoFields["lastchange"]->setInfo("---");
  infoFields["devicename"]->setInfo("---");
  infoFields["uiversion"]->setInfo("---");
  return true;
}

Glib::ustring PresetInfoContent::createPresetTypeString(const Preset *preset)
{
  Glib::ustring type = toString(preset->getType());

  auto monoI = preset->findParameterByID({ 364, VoiceGroup::I }, false);
  auto monoII = preset->findParameterByID({ 364, VoiceGroup::II }, false);
  auto unisonI = preset->findParameterByID({ 249, VoiceGroup::I }, true);
  auto unisonII = preset->findParameterByID({ 249, VoiceGroup::II }, true);

  const auto monoIEnabled = monoI ? differs(monoI->getValue(), 0.0) : false;
  const auto monoIIEnabled = monoII ? differs(monoII->getValue(), 0.0) : false;

  const auto unisonIEnabled = differs(unisonI->getValue(), 0.0);
  const auto unisonIIEnabled = differs(unisonII->getValue(), 0.0);

  auto createSuffixedString = [&](const std::string &prefix, auto I, auto II) -> std::string {
    if(I && II)
      return prefix + " I/II";
    else if(I)
      return prefix + " I";
    else if(II)
      return prefix + " II";
    else
      return "-";
  };

  auto unisonString = createSuffixedString("Unison", unisonIEnabled, unisonIIEnabled);
  auto monoString = createSuffixedString("Mono", monoIEnabled, monoIIEnabled);

  auto monoEnabled = monoIEnabled || monoIIEnabled;
  auto unisonEnabled = unisonIEnabled || unisonIIEnabled;
  auto monoAndUnisonEnabled = monoEnabled && unisonEnabled;

  switch(preset->getType())
  {
    default:
    case SoundType::Single:
    case SoundType::Layer:
      if(monoAndUnisonEnabled)
        return type + " (Mono, Unison)";
      else if(monoEnabled)
        return type + " (Mono)";
      else if(unisonEnabled)
        return type + " (Unison)";
      else
        return type;
    case SoundType::Split:
    {
      if(monoAndUnisonEnabled)
        return type + " (" + monoString + ", " + unisonString + ")";
      else if(monoEnabled)
        return type + " (" + monoString + ")";
      else if(unisonEnabled)
        return type + " (" + unisonString + ")";
      else
        return type;
    }
  }
}
