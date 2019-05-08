#include "RenameMCLayout.h"
#include "Application.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>

RenameMCLayout::RenameMCLayout()
    : super()
{
}

void RenameMCLayout::commit(const Glib::ustring &newName)
{
  if(auto p = getMacroControl())
  {
    p->undoableSetGivenName(newName);
  }
}

Glib::ustring RenameMCLayout::getInitialText() const
{
  if(auto p = getMacroControl())
  {
    return p->getGivenName();
  }

  return "";
}

MacroControlParameter *RenameMCLayout::getMacroControl() const
{
  return dynamic_cast<MacroControlParameter *>(
      Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter());
}
