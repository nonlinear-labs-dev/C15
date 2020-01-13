package com.nonlinearlabs.client.world.maps.parameters;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.SVFilter.LittleKnobSlider;

public class SmallHorizontalDotSlider extends ModulationSourceHighPriority {

    public SmallHorizontalDotSlider(MapsLayout parent, int parameterId) {
        super(parent, parameterId);
        addChild(new LabelModulationSource(this, getParameterNumber()));
        addChild(new LittleKnobSlider(this, getParameterNumber()));
        addChild(new ValueDisplaySmall(this, getParameterNumber()));
    }
}