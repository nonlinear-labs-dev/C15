#include <groups/ScaleGroup.h>
#include <parameters/ScaleParameter.h>
#include <parameters/scale-converters/BaseKeyScaleConverter.h>
#include <parameters/scale-converters/KeyScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>

ScaleGroup::ScaleGroup(ParameterDualGroupSet* parent)
    : ParameterGroup(parent, { "Scale", VoiceGroup::Global }, "Scale", "Scale", "Scale")
    , m_updateNames(std::chrono::milliseconds(200))
{
}

ScaleGroup::~ScaleGroup()
{
}

bool ScaleGroup::isAnyOffsetChanged() const
{
  for(const auto p : getParameters())
    if(p->getID().getNumber() != getScaleBaseParameterNumber())
      if(p->getControlPositionValue() != p->getDefaultValue())
        return true;

  return false;
}

int ScaleGroup::getScaleBaseParameterNumber()
{
  return 312;
}

void ScaleGroup::init()
{
  auto baseKeyParam = new ScaleParameter(this, { getScaleBaseParameterNumber(), VoiceGroup::Global },
                                         ScaleConverter::get<BaseKeyScaleConverter>(), 0, 11, 11);
  baseKeyParam->onParameterChanged(mem_fun(this, &ScaleGroup::onBaseKeyParameterChanged), false);
  appendParameter(baseKeyParam);

  appendParameter(
      new ScaleParameter(this, { 313, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 314, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 315, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 316, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 317, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 318, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 319, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 320, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 321, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 322, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 323, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(
      new ScaleParameter(this, { 391, VoiceGroup::Global }, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
}

void ScaleGroup::onBaseKeyParameterChanged(const Parameter* baseKeyParameter)
{
  m_updateNames.doTask([=]() {
    for(auto a : getParameters())
      a->onChange();
  });
}

bool ScaleGroup::isScaleParameter(const ParameterId& id)
{
  auto number = id.getNumber();
  return (number >= 312 && number <= 323) || number == 391;
}

bool ScaleGroup::isScaleParameter(const Parameter* parameter)
{
  if(parameter)
    return isScaleParameter(parameter->getID());
  return false;
}