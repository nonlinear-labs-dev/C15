package com.nonlinearlabs.client.world.maps.parameters.VoiceGroup;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.LabelLarge;
import com.nonlinearlabs.client.world.maps.parameters.NumericalControlSmall;
import com.nonlinearlabs.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;

public class VoiceGroupColumn extends ParameterColumn {
    private class Volume extends Parameter {

        private Volume(MapsLayout parent) {
            super(parent);
            addChild(new LabelLarge(this, getName()));
            addChild(new NumericalControlSmall(this));
        }

        @Override
        public int getParameterID() {
            return 249;
        }
    }

    private class Tune extends Parameter {

        private Tune(MapsLayout parent) {
            super(parent);
            addChild(new LabelLarge(this, getName()));
            addChild(new NumericalControlSmall(this));
        }

        @Override
        public int getParameterID() {
            return 249;
        }
    }

    public VoiceGroupColumn(MapsLayout parent) {
        super(parent);
        addChild(new Volume(this));
        addChild(new Tune(this));
    }

}
