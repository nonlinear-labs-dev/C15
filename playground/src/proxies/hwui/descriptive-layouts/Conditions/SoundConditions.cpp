#include "SoundConditions.h"

DescriptiveLayouts::SoundConditions::EditBufferCondition::EditBufferCondition()
{
  m_editbufferChangedConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &EditBufferCondition::onEditBufferChanged));
}

void DescriptiveLayouts::SoundConditions::EditBufferCondition::onEditBufferChanged()
{
  get();
}

DescriptiveLayouts::SoundConditions::EditBufferCondition::~EditBufferCondition()
{
  m_editbufferChangedConnection.disconnect();
}

bool DescriptiveLayouts::SoundConditions::IsSingleSound::check() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBufferType::Single;
}

bool DescriptiveLayouts::SoundConditions::IsLayerSound::check() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBufferType::Layer;
}

bool DescriptiveLayouts::SoundConditions::IsSplitSound::check() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBufferType::Split;
}