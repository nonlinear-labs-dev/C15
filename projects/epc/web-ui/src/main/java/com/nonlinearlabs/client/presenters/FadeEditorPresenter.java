package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;

public class FadeEditorPresenter {

    public class KeyRange {
        public int from;
        public int to;
        public int indicator;
    }

    public KeyRange splitI = new KeyRange();
    public KeyRange splitII = new KeyRange();

    public KeyRange fadePointI = new KeyRange();
    public KeyRange fadePointII = new KeyRange();

    public double fadeRangeI;
    public double fadeRangeII;

    public RGB colorVGI = new RGB(0x26, 0xb0, 0xff);
    public RGBA colorVGIFill = new RGBA(colorVGI, 0.5);
    public RGB colorVGII = new RGB(0xff, 0x99, 0x33);
    public RGBA colorVGIIFill = new RGBA(colorVGII, 0.5);

    public String fadeRangeTextI;
    public String fadeRangeTextII;

    public String fadePointTextI;
    public String fadePointTextII;

    public String splitPointTextI;
    public String splitPointTextII;

    public double getFadeRangeValue(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return fadeRangeI;
        } else {
            return fadeRangeII;
        }
    }

    public FadeEditorPresenter.KeyRange getSplitRange(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return splitI;
        } else {
            return splitII;
        }
    }

    public FadeEditorPresenter.KeyRange getFadePointRange(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return fadePointI;
        } else {
            return fadePointII;
        }
    }

    public double getFadeRangePos(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return fadeRangeI;
        } else {
            return fadeRangeII;
        }
    }

    public RGBA getStrokeColor(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return new RGBA(colorVGI, 1);
        } else {
            return new RGBA(colorVGII, 1);
        }
    }

    public RGBA getFillColor(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return colorVGIFill;
        } else {
            return colorVGIIFill;
        }
    }

    public String getFadeRangeText(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return fadeRangeTextI;
        } else {
            return fadeRangeTextII;
        }
    }

    public String getFadePointText(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return fadePointTextI;
        } else {
            return fadePointTextII;
        }
    }

    public String getSplitPointText(VoiceGroup vg) {
        if (vg == VoiceGroup.I) {
            return splitPointTextI;
        } else {
            return splitPointTextII;
        }
    }

    public VoiceGroup lastSelectedPart = VoiceGroup.I;
}
