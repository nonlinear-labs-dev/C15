#pragma once

#include <presets/PresetParameter.h>

class Parameter;

class ParameterRecallData
{
public:
    ParameterRecallData(Parameter *p);
    void onLoad(UNDO::Transaction* transaction);
    void onSave(UNDO::Transaction* transaction);
    const PresetParameter& getParameterCache() const;
protected:
    PresetParameter m_presetParameter;
};