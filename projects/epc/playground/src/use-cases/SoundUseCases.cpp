#include "SoundUseCases.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <libundo/undo/Scope.h>

SoundUseCases::SoundUseCases(EditBuffer* eb, PresetManager* pm)
    : m_editBuffer { eb }
    , m_presetManager { pm }
{
}

void SoundUseCases::storeInitSound()
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Store Init Sound");
  m_presetManager->storeInitSound(scope->getTransaction());
}

void SoundUseCases::resetInitSound()
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Reset Init Sound");
  m_presetManager->resetInitSound(scope->getTransaction());
}

void SoundUseCases::randomizeSound()
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Randomize Sound");
  m_presetManager->getEditBuffer()->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_HWUI);
}

void SoundUseCases::randomizePart(VoiceGroup part)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Randomize Part");
  m_editBuffer->undoableRandomizePart(scope->getTransaction(), part, Initiator::EXPLICIT_WEBUI);
}

void SoundUseCases::initSound()
{
  auto scope = m_editBuffer->getUndoScope().startTransaction("Init Sound");
  m_editBuffer->undoableInitSound(scope->getTransaction(), Defaults::UserDefault);
}

void SoundUseCases::initPart(VoiceGroup part)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Init Part");
  m_editBuffer->undoableInitPart(scope->getTransaction(), part, Defaults::UserDefault);
}

void SoundUseCases::renamePart(VoiceGroup part, const Glib::ustring& name)
{
  auto scope = m_presetManager->getUndoScope().startTransaction("Rename Part");
  m_editBuffer->setVoiceGroupName(scope->getTransaction(), name, part);
}

void SoundUseCases::convertToLayer()
{
  auto scope = m_editBuffer->getUndoScope().startTransaction("Convert Sound to Layer");
  auto transaction = scope->getTransaction();
  m_editBuffer->undoableConvertToDual(transaction, SoundType::Layer);
}

void SoundUseCases::convertToSingle(VoiceGroup partToUse)
{
  auto scope = m_editBuffer->getUndoScope().startTransaction("Convert Sound to Single");
  auto transaction = scope->getTransaction();
  m_editBuffer->undoableConvertToSingle(transaction, partToUse);
}

void SoundUseCases::convertToSplit()
{
  auto scope = m_editBuffer->getUndoScope().startTransaction("Convert Sound to Split");
  auto transaction = scope->getTransaction();
  m_editBuffer->undoableConvertToDual(transaction, SoundType::Split);
}