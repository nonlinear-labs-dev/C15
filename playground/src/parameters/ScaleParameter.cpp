#include <groups/ParameterGroup.h>
#include <groups/ScaleGroup.h>
#include <libundo/undo/StringTools.h>
#include <parameters/ScaleParameter.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ScaleParameterLayout.h>
#include <xml/Writer.h>
#include "scale-converters/dimension/NoteDimension.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

ScaleParameter::ScaleParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling,
                               tControlPositionValue def, tControlPositionValue coarseDenominator,
                               tControlPositionValue fineDenominator)
    : Parameter(group, id, scaling, def, coarseDenominator, fineDenominator)
{
}

DFBLayout *ScaleParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new ScaleParameterEditLayout();

    case UIMode::Select:
    default:
      return new ScaleParameterSelectLayout();
  }

  g_return_val_if_reached(nullptr);
}

Glib::ustring ScaleParameter::getMiniParameterEditorName() const
{
  return "Scale...";
}

void ScaleParameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  super::writeDocProperties(writer, knownRevision);

  if(!shouldWriteDocProperties(knownRevision))
  {
    writer.writeTextElement("long-name", getLongName());
  }
}

size_t ScaleParameter::getHash() const
{
  size_t hash = super::getHash();
  hash_combine(hash, getLongName());
  return hash;
}

Glib::ustring ScaleParameter::getLongName() const
{
  static const auto baseKeyParameterID = ParameterId(312, VoiceGroup::Global);

  const auto baseKey = getParentGroup()->getParameterByID(baseKeyParameterID);
  auto offset = 0;
  if(getID().getNumber() != 391)
    offset = getID().getNumber() - baseKeyParameterID.getNumber();

  if(getID() != baseKeyParameterID)
  {
    const auto key = static_cast<int>(baseKey->getDisplayValue() + offset);
    const auto stringizedKey = NoteDimension::get().stringize(key % 12);
    return UNDO::StringTools::buildString(super::getLongName(), " (", stringizedKey, ")");
  }
  return super::getLongName();
}
