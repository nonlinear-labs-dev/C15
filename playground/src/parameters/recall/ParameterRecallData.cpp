#include "ParameterRecallData.h"
#include "presets/Preset.h"
#include "parameters/Parameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "Application.h"


ParameterRecallData::ParameterRecallData(Parameter *p)
    : m_presetParameter{ p->getID() }
{
}

void ParameterRecallData::onLoad(UNDO::Transaction* transaction)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  if(auto origin = eb->getOrigin())
    m_presetParameter.copyFrom(transaction, origin->findParameterByID(m_presetParameter.getID()));
  else
    m_presetParameter.copyFrom(transaction, eb->findParameterByID(m_presetParameter.getID()));
}

void ParameterRecallData::onSave(UNDO::Transaction* transaction)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  m_presetParameter.copyFrom(transaction, eb->findParameterByID(m_presetParameter.getID()));
}

const PresetParameter& ParameterRecallData::getParameterCache() const
{
    return m_presetParameter;
}
