#include "MockPresetStorage.h"
#include <testing/TestHelper.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <presets/EditBuffer.h>

inline EditBuffer *getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

MockPresetStorage::MockPresetStorage()
{
  auto scope = UNDO::Scope::startTrashTransaction();
  auto transaction = scope->getTransaction();

  m_bank = createBank(transaction);

  m_layer = createLayerPreset(transaction);
  m_split = createSplitPreset(transaction);
  m_single = createSinglePreset(transaction);
}

MockPresetStorage::~MockPresetStorage()
{
  auto scope = UNDO::Scope::startTrashTransaction();
  auto transaction = scope->getTransaction();
  auto editBuffer = getEditBuffer();
  auto pm = editBuffer->getParent();
  pm->deleteBank(transaction, m_bank->getUuid());
}

Preset *MockPresetStorage::getSinglePreset()
{
  return m_single;
}

Preset *MockPresetStorage::getSplitPreset()
{
  return m_split;
}

Preset *MockPresetStorage::getLayerPreset()
{
  return m_layer;
}

Preset *MockPresetStorage::createSinglePreset(UNDO::Transaction *transaction)
{
  auto editBuffer = getEditBuffer();
  editBuffer->undoableConvertToSingle(transaction, VoiceGroup::I);
  auto preset = m_bank->appendPreset(transaction, std::make_unique<Preset>(m_bank, *editBuffer));
  preset->setName(transaction, "Single Preset");
  return preset;
}

Preset *MockPresetStorage::createSplitPreset(UNDO::Transaction *transaction)
{
  auto editBuffer = getEditBuffer();
  editBuffer->undoableConvertToSingle(transaction, VoiceGroup::I);
  editBuffer->undoableConvertToDual(transaction, SoundType::Split);
  auto preset = m_bank->appendPreset(transaction, std::make_unique<Preset>(m_bank, *editBuffer));
  preset->setName(transaction, "Split Preset");
  return preset;
}

Preset *MockPresetStorage::createLayerPreset(UNDO::Transaction *transaction)
{
  auto editBuffer = getEditBuffer();
  editBuffer->undoableConvertToDual(transaction, SoundType::Layer);
  auto preset = m_bank->appendPreset(transaction, std::make_unique<Preset>(m_bank, *editBuffer));
  preset->setName(transaction, "Layer Preset");
  return preset;
}

Bank *MockPresetStorage::createBank(UNDO::Transaction *transaction)
{
  auto eb = getEditBuffer();
  auto pm = eb->getParent();
  return pm->addBank(transaction);
}