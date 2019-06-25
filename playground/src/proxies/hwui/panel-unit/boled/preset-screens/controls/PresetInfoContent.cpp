#include <Application.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetInfoContent.h>
#include <iomanip>
#include <sstream>
#include <tools/TimeTools.h>

namespace DETAIL
{
  class PresetComment : public MultiLineLabel
  {
   public:
    PresetComment()
        : MultiLineLabel("---")
    {
    }

    Oleds::tFont getFont()
    {
      return Oleds::get().getFont("Emphase_8_TXT_Regular", 8);
    }
  };
}

static const int divider = 64;

PresetInfoContent::PresetInfoContent()
{
  addInfoField("name", "Name", new MultiLineContent());
  addInfoField("comment", "Comment", new MultiLineContent());
  addInfoField("lastchange", "Last Change");
  addInfoField("devicename", "Device Name");
  addInfoField("uiversion", "UI Version");

  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &PresetInfoContent::onBankChanged));
}

PresetInfoContent::~PresetInfoContent()
{
}

void PresetInfoContent::onBankChanged(const Uuid &selectedBank)
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
  if(auto preset = getCurrentPreset())
  {
    if(fillFromPreset(preset))
    {
      fixLayout();
    }
  }
}

Preset *PresetInfoContent::getCurrentPreset()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
    return bank->getSelectedPreset();

  return nullptr;
}

bool PresetInfoContent::fillFromPreset(const Preset *preset)
{
  infoFields["name"]->setInfo(preset->getName(), FrameBuffer::Colors::C128);
  infoFields["comment"]->setInfo(preset->getAttribute("Comment", "---"), FrameBuffer::Colors::C128);
  infoFields["lastchange"]->setInfo(TimeTools::getDisplayStringFromIso(preset->getAttribute("StoreTime", "---")));
  infoFields["devicename"]->setInfo(preset->getAttribute("DeviceName", "---"));
  infoFields["uiversion"]->setInfo(preset->getAttribute("SoftwareVersion", "---"));
  return true;
}

bool PresetInfoContent::fillDefaults()
{
  infoFields["name"]->setInfo("---", FrameBuffer::Colors::C128);
  infoFields["comment"]->setInfo("---", FrameBuffer::Colors::C128);
  infoFields["lastchange"]->setInfo("---");
  infoFields["devicename"]->setInfo("---");
  infoFields["uiversion"]->setInfo("---");
  return true;
}

void PresetInfoContent::fixLayout()
{
  int y = 0;

  for(auto infoKey : { "name", "comment", "lastchange", "devicename", "uiversion" })
  {
    y = infoFields[infoKey]->format(y);
  }

  Rect r = getPosition();
  r.setHeight(y);
  super::setPosition(r);
}
