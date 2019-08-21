//
// Created by justus on 21.08.19.
//

#include "InitSoundItem.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

InitSound::InitSound()
    : BasicItem("Init")
{
}

void InitSound::doAction()
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Init Sound");
  editBuffer->undoableInitSound(scope->getTransaction());
}
