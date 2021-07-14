package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.presenters.ParameterPresenterProviders;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class FadeIndicator extends SVGImage {
    public FadeIndicator(Control parent, VoiceGroup vg) {
        super(parent, "Part-Fade-Ind_A.svg", "Part_Fade-Ind_B.svg", "Part_Fade-Ind_C.svg");
        ParameterPresenterProviders.get().register(new ParameterId(396, vg), presenter -> {
            if (presenter.isDefault)
                selectPhase(0);
            else
                selectPhase(vg == VoiceGroup.I ? 1 : 2);
                
            return true;
        });
    }
}