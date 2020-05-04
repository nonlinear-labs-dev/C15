package com.nonlinearlabs.client.world.maps.parameters.FBMixer;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableHorizontalSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

public class FBMixerColumn1 extends ParameterColumn {

    public FBMixerColumn1(MapsLayout parent) {
        super(parent);
        addChild(new ModulatableSlider(this, 346));
        addChild(new ModulatableHorizontalSlider(this, 348));
    }

}
