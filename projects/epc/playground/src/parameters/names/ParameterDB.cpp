#include <Application.h>
#include <playground.h>
#include <boost/tokenizer.hpp>
#include <parameters/names/ParameterDB.h>
#include <tools/StringTools.h>
#include <parameters/Parameter.h>
#include <parameter_list.h>
#include <cassert>
#include <nltools/logging/Log.h>
#include <groups/MacroControlsGroup.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>

ParameterDB &ParameterDB::get()
{
  static ParameterDB db;
  return db;
}

ParameterDB::ParameterDB() = default;

ParameterDB::~ParameterDB() = default;

std::string sanitize(const std::string &in)
{
  auto mod = StringTools::replaceAll(in, "Ⓐ", u8"\ue000");
  mod = StringTools::replaceAll(mod, "Ⓑ", u8"\ue001");
  mod = StringTools::replaceAll(mod, "Ⓒ", u8"\ue002");
  mod = StringTools::replaceAll(mod, "Ⓓ", u8"\ue003");
  mod = StringTools::replaceAll(mod, "Ⓔ", u8"\ue200");
  mod = StringTools::replaceAll(mod, "Ⓕ", u8"\ue201");
  return mod;
}

Glib::ustring ParameterDB::getLongName(const ParameterId &id) const
{
  auto num = id.getNumber();

  assert(num >= 0);
  assert(num < C15::Config::tcd_elements);

  auto d = C15::ParameterList[num];
  if(!d.m_pg.m_param_label_long)
  {
    nltools::Log::error("there is no long name entry in parameter list for parameter", num);
    return "MISSING!!!";
  }

  return replaceVoiceGroupInDynamicLabels(d.m_pg.m_param_label_long, id.getVoiceGroup());
}

Glib::ustring ParameterDB::getShortName(const ParameterId &id) const
{
  auto num = id.getNumber();

  assert(num >= 0);
  assert(num < C15::Config::tcd_elements);

  auto d = C15::ParameterList[num];
  if(!d.m_pg.m_param_label_short)
  {
    nltools::Log::error("there is no short name entry in parameter list for parameter", num);
    return "MISSING!!!";
  }

  return replaceVoiceGroupInDynamicLabels(d.m_pg.m_param_label_short, id.getVoiceGroup());
}

Glib::ustring ParameterDB::getDescription(const ParameterId &id) const
{
  auto num = id.getNumber();
  return getDescription(num);
}

std::optional<Glib::ustring> ParameterDB::getLongGroupName(const ParameterId &id) const
{
  auto d = C15::ParameterList[id.getNumber()];

  if(!d.m_pg.m_group_label_long)
    return {};

  return replaceVoiceGroupInDynamicLabels(d.m_pg.m_group_label_long, id.getVoiceGroup());
}

Glib::ustring ParameterDB::getDescription(const int num) const
{
  assert(num >= 0);
  assert(num < C15::Config::tcd_elements);

  if(MacroControlsGroup::isMacroControl(num))
  {
    auto param = Application::get().getPresetManager()->getEditBuffer()->findParameterByID({ num, VoiceGroup::Global });
    if(auto mcParam = dynamic_cast<const MacroControlParameter *>(param))
    {
      return mcParam->getInfo();
    }
  }

  auto d = C15::ParameterList[num];
  if(!d.m_pg.m_param_info)
  {
    nltools::Log::error("there is no short name entry in parameter list for parameter", num);
    return "";
  }

  return d.m_pg.m_param_info;
}

tControlPositionValue ParameterDB::getSignalPathIndication(int id) const
{
  assert(id >= 0);
  assert(id < C15::Config::tcd_elements);

  auto d = C15::ParameterList[id];
  return (d.m_pg.m_inactive_cp && strlen(d.m_pg.m_inactive_cp) > 0) ? std::stod(d.m_pg.m_inactive_cp)
                                                                    : getInvalidSignalPathIndication();
}

Glib::ustring ParameterDB::replaceVoiceGroupInDynamicLabels(Glib::ustring name, VoiceGroup originGroup) const
{
  if(name.find("@VG") != Glib::ustring::npos)
  {
    auto invert = [](VoiceGroup v) { return v == VoiceGroup::I ? VoiceGroup::II : VoiceGroup::I; };
    name.replace(name.find("@VG"), 3, toString(invert(originGroup)));
    return name;
  }

  return name;
}
