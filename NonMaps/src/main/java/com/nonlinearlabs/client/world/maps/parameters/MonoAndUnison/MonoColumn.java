package com.nonlinearlabs.client.world.maps.parameters.MonoAndUnison;

import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.parameters.BooleanControlSmall;
import com.nonlinearlabs.client.world.maps.parameters.ModulateableKnob;
import com.nonlinearlabs.client.world.maps.parameters.NumericalControlSmall;
import com.nonlinearlabs.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.client.world.maps.parameters.ParameterColumn;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroup;
import com.nonlinearlabs.client.world.maps.parameters.UnModulateableParameterName;

public class MonoColumn extends ParameterColumn {
    private class Enable extends Parameter {

        private Enable(MapsLayout parent) {
            super(parent, 364);
            addChild(new UnModulateableParameterName(this));
            addChild(new BooleanControlSmall(this, getParameterNumber()));
        }
    }

    private class PriorityControlSmall extends NumericalControlSmall {

        public PriorityControlSmall(MapsLayout parent, int parameterID) {
            super(parent, parameterID);
        }
    }

    private class Priority extends Parameter {
        private Priority(MapsLayout parent) {
            super(parent, 365);
            addChild(new UnModulateableParameterName(this));
            addChild(new PriorityControlSmall(this, getParameterNumber()));
        }
    }

    private class Legato extends Parameter {
        private Legato(MapsLayout parent) {
            super(parent, 366);
            addChild(new UnModulateableParameterName(this));
            addChild(new BooleanControlSmall(this, getParameterNumber()));
        }
    }

    public MonoColumn(ParameterGroup parent) {
        super(parent);
        addChild(new Enable(this));
        addChild(new Priority(this));
        addChild(new Legato(this));
        addChild(new ModulateableKnob(this, 367));
    }
}
