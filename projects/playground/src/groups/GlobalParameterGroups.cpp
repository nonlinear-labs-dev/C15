#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48StScaleConverter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterModulateableParameter.h>
#include <parameters/SplitPointParameter.h>
#include "GlobalParameterGroups.h"

GlobalParameterGroups::GlobalParameterGroups(ParameterDualGroupSet *parent)
    : ParameterGroup(parent, { "Split", VoiceGroup::Global }, "Split", "Split", "Split")
{
}

void GlobalParameterGroups::init()
{
  appendParameter(new SplitPointParameter(this, { 356, VoiceGroup::Global }));
}

bool GlobalParameterGroups::isSplitPoint(const Parameter *p)
{
  return isSplitPoint(p->getID());
}

bool GlobalParameterGroups::isSplitPoint(const ParameterId &id)
{
  return id.getNumber() == 356 && id.getVoiceGroup() == VoiceGroup::Global;
}
