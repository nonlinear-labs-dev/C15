#include "EditMCInfoLayout.h"
#include "Application.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>

EditMCInfoLayout::EditMCInfoLayout () :
    super ()
{
}

void EditMCInfoLayout::commit (const Glib::ustring &newName)
{
  if (auto p = getMacroControl ())
  {
    p->undoableSetInfo (newName);
  }
}

Glib::ustring EditMCInfoLayout::getInitialText () const
{
  if (auto p = getMacroControl ())
  {
    return p->getInfo ();
  }

  return "";
}

MacroControlParameter *EditMCInfoLayout::getMacroControl () const
{
  return dynamic_cast<MacroControlParameter*> (Application::get().getPresetManager()->getEditBuffer()->getSelectedParameter());
}
