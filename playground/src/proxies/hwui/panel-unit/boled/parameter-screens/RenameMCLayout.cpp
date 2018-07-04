#include "RenameMCLayout.h"
#include "Application.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>
#include <proxies/hwui/HWUI.h>


RenameMCLayout::RenameMCLayout () :
    super ()
{
}

RenameMCLayout::~RenameMCLayout() {
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().resetOverlay();
  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Parameters, UIMode::Edit));
}

void RenameMCLayout::commit (const Glib::ustring &newName)
{
  if (auto p = getMacroControl ())
  {
    p->undoableSetGivenName (newName);
  }
}

Glib::ustring RenameMCLayout::getInitialText () const
{
  if (auto p = getMacroControl ())
  {
    return p->getGivenName ();
  }

  return "";
}

MacroControlParameter *RenameMCLayout::getMacroControl () const
{
  return dynamic_cast<MacroControlParameter*> (Application::get ().getPresetManager ()->getEditBuffer ()->getSelected ());
}
