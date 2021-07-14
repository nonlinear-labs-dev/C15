package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.FadePointKeyBed;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.KeyBed;

public class FadePointsKeyBedEditor extends KeyBedEditor {

    private class LayerPartIndicators extends OverlayLayout {

        private VoiceGroup voiceGroup;
        private ValueDisplay fadePoint;
        private ValueDisplay fadeRange;

        LayerPartIndicators(OverlayLayout p, VoiceGroup vg) {
            super(p);
            voiceGroup = vg;

            fadePoint = addChild(new ValueDisplay(this, new ParameterId(396, voiceGroup)));
            fadeRange = addChild(new ValueDisplay(this, new ParameterId(397, voiceGroup)));
        }

        @Override
        public void doLayout(double x, double y, double w, double h) {
            super.doLayout(x, y, w, h);

            double pH = h / 4;
            fadePoint.doLayout(0, h / 3 * 1 - pH / 2, w, pH);
            fadeRange.doLayout(0, h / 3 * 2 - pH / 2, w, pH);
        }
    }

    private OverlayControl leftControls = null;
    private OverlayControl rightControls = null;
    private KeyBed keys;
    private HorizonatlScrollPane keysPane;

    protected FadePointsKeyBedEditor(Control parent) {
        super(parent);

        keysPane = addChild(new HorizonatlScrollPane(this, keys = new FadePointKeyBed(this)));
        leftControls = addChild(new LayerPartIndicators(this, VoiceGroup.I));
        rightControls = addChild(new LayerPartIndicators(this, VoiceGroup.II));
    }

    @Override
    public void doLayout(double x, double y, double w, double h) {
        super.doLayout(x, y, w, h);

        double settingsWidth = closeButton.getPictureWidth();
        double settingsHeight = closeButton.getPictureHeight();
        double xMargin = settingsWidth * 0.5;

        int parts = 8;
        double partWidth = w / parts;
        double keysWidth = Math.min(keys.getPictureWidth(), partWidth * 5);
        keysPane.doLayout(x + (w / 2) - (keysWidth / 2), y, keysWidth, h);

        double valueWidth = Math.max(partWidth * 0.75, Millimeter.toPixels(10));
        leftControls.doLayout(keysPane.getRelativePosition().getLeft() - valueWidth - xMargin, 0, valueWidth, h);        
        rightControls.doLayout(keysPane.getRelativePosition().getRight() + xMargin, 0, valueWidth, h);

        double closeXAvaliableSpace = w - rightControls.getRelativePosition().getRight();
        double closeX = rightControls.getRelativePosition().getRight() + (closeXAvaliableSpace / 2) - settingsWidth / 2;

        closeButton.doLayout(closeX, h / 2 - settingsHeight / 2, settingsWidth, settingsHeight);
    }
}
