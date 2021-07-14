#include <Application.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include "RenameVoiceGroupLayout.h"
#include <libundo/undo/Scope.h>

inline EditBuffer* getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

RenameVoiceGroupLayout::RenameVoiceGroupLayout(VoiceGroup vg)
    : m_targetGroup { vg }
{
}

void RenameVoiceGroupLayout::cancel()
{
  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Sound, UIMode::Select, UIDetail::Init));
}

void RenameVoiceGroupLayout::commit(const Glib::ustring& newName)
{
  auto eb = getEditBuffer();
  EditBufferUseCases useCase(eb);
  useCase.renamePart(m_targetGroup, newName);

  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Sound, UIMode::Select, UIDetail::Init));
}

Glib::ustring RenameVoiceGroupLayout::getInitialText() const
{
  return getEditBuffer()->getVoiceGroupName(m_targetGroup);
}
