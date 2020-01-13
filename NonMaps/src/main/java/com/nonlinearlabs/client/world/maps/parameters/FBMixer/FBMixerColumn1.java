package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallHorizontalDotSlider;

public class FBMixerColumn1 extends ParameterColumn {

    public FBMixerColumn1(MapsLayout parent) {
        super(parent);
        addChild(new ModulatableSlider(this, 346));
        addChild(new SmallHorizontalDotSlider(this, 348));
    }

}
