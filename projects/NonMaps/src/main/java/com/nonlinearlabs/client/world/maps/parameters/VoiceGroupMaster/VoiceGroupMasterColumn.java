package com.nonlinearlabs.client.world.maps.parameters.VoiceGroupMaster;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableHorizontalSlider;
import com.nonlinearlabs.client.world.maps.parameters.ModulatableSlider;
import com.nonlinearlabs.client.world.maps.parameters.NumericalControlSmall;
import com.nonlinearlabs.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.SmallModulateableParameter;
import com.nonlinearlabs.client.world.maps.parameters.UnModulateableParameterName;

public class VoiceGroupMasterColumn extends ParameterColumn {

    private class UnmodulateablePartMasterParameter extends Parameter {

        private UnmodulateablePartMasterParameter(MapsLayout parent, int num) {
            super(parent, num);
            addChild(new UnModulateableParameterName(this));
            addChild(new NumericalControlSmall(this, getParameterNumber()));
        }
    }

    public VoiceGroupMasterColumn(MapsLayout parent) {
        super(parent);
        addChild(new ModulatableSlider(this, 358));
        addChild(new SmallModulateableParameter(this, 360));
        addChild(new UnmodulateablePartMasterParameter(this, 396));
        addChild(new UnmodulateablePartMasterParameter(this, 397));
        addChild(new ModulatableHorizontalSlider(this, 356));
    }

}
