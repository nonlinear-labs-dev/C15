package com.nonlinearlabs.client.world.maps.parameters;

import com.nonlinearlabs.client.world.maps.MapsLayout;

public class SmallModulateableParameter extends ModulatableParameter {

        public SmallModulateableParameter(MapsLayout parent, int num) {
            super(parent, num);
            addChild(new ModulationSourceLabel(this, getParameterNumber()));
            addChild(new NumericalControlSmall(this, getParameterNumber()) {
                @Override
                protected double getInsetY() {
                    return 0;
                }

                @Override
                protected double getBasicHeight() {
                    return 14;
                }
            });

            addChild(new Spacer(this, 1, 6));
        }

    }