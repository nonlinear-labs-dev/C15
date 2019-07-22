#include "SoundConditions.h"

DescriptiveLayouts::SoundConditions::IsXSound::IsXSound()
    : m_changed{ this }
{
}
void DescriptiveLayouts::SoundConditions::IsXSound::onEditBufferChanged(const EditBuffer *)
{
  get();
}

bool DescriptiveLayouts::SoundConditions::IsSingleSound::check() const {
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBuffer::Type::Single;
}

bool DescriptiveLayouts::SoundConditions::IsLayerSound::check() const {
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBuffer::Type::Layer;
}

bool DescriptiveLayouts::SoundConditions::IsSplitSound::check() const {
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getType() == EditBuffer::Type::Split;
}
