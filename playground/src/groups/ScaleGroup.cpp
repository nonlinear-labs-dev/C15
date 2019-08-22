#include <groups/ScaleGroup.h>
#include <parameters/ScaleParameter.h>
#include <parameters/scale-converters/BaseKeyScaleConverter.h>
#include <parameters/scale-converters/KeyScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>

ScaleGroup::ScaleGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "Scale", "Scale", "Scale", "Scale")
    , m_updateNames(std::chrono::milliseconds(200))
{
}

ScaleGroup::~ScaleGroup()
{
}

bool ScaleGroup::isAnyOffsetChanged() const
{
  for(const auto p : getParameters())
    if(p->getID() != getScaleBaseParameterID())
      if(p->getControlPositionValue() != p->getDefaultValue())
        return true;

  return false;
}

int ScaleGroup::getScaleBaseParameterID()
{
  return 312;
}

void ScaleGroup::init()
{
  auto baseKeyParam
      = new ScaleParameter(this, getScaleBaseParameterID(), ScaleConverter::get<BaseKeyScaleConverter>(), 0, 11, 11);
  baseKeyParam->onParameterChanged(mem_fun(this, &ScaleGroup::onBaseKeyParameterChanged), false);
  appendParameter(baseKeyParam);

  appendParameter(new ScaleParameter(this, 313, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 314, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 315, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 316, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 317, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 318, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 319, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 320, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 321, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 322, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
  appendParameter(new ScaleParameter(this, 323, ScaleConverter::get<KeyScaleConverter>(), 0, 800, 8000));
}

void ScaleGroup::onBaseKeyParameterChanged(const Parameter *baseKeyParameter)
{
  m_updateNames.doTask([=]() {
    for(auto a : getParameters())
      a->onChange();
  });
}
