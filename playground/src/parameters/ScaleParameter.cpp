#include <groups/ParameterGroup.h>
#include <libundo/undo/StringTools.h>
#include <parameters/ScaleParameter.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ScaleParameterLayout.h>
#include <xml/Writer.h>
#include "scale-converters/dimension/NoteDimension.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

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
  static const int baseKeyParameterID = 312;

  if(getID() != baseKeyParameterID)
  {
    const auto baseKey = getParentGroup()->getParameterByID(baseKeyParameterID);
    const auto offset = getID() - baseKeyParameterID;
    const auto key = static_cast<int>(baseKey->getDisplayValue() + offset);
    const auto stringizedKey = NoteDimension::get().stringize(key % 12);
    return UNDO::StringTools::buildString(super::getLongName(), " (", stringizedKey, ")");
  }
  return super::getLongName();
}
