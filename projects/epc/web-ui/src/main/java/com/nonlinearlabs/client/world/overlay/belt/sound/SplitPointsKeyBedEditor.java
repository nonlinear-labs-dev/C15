package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.KeyBed;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.SplitPointKeyBed;
import com.nonlinearlabs.client.world.overlay.belt.parameters.SyncParameterButton;

public class SplitPointsKeyBedEditor extends KeyBedEditor {

    private SyncParameterButton button;
    private OverlayControl leftControls = null;
    private OverlayControl rightControls = null;
    private KeyBed keys;
    private HorizonatlScrollPane keysPane;

    protected SplitPointsKeyBedEditor(Control parent) {
        super(parent);

        keysPane = addChild(new HorizonatlScrollPane(this, keys = new SplitPointKeyBed(this)));
        button = addChild(new SyncParameterButton(this));
        leftControls = addChild(new ValueDisplay(this, new ParameterId(356, VoiceGroup.I)));
        rightControls = addChild(new ValueDisplay(this, new ParameterId(356, VoiceGroup.II)));
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
        leftControls.doLayout(keysPane.getRelativePosition().getLeft() - valueWidth - xMargin, h / 2 - settingsHeight / 2, valueWidth, settingsHeight);        
        rightControls.doLayout(keysPane.getRelativePosition().getRight() + xMargin, h / 2 - settingsHeight / 2, valueWidth, settingsHeight);

        double closeXAvaliableSpace = w - rightControls.getRelativePosition().getRight();
        double closeX = rightControls.getRelativePosition().getRight() + (closeXAvaliableSpace / 2);

        closeButton.doLayout(closeX - settingsWidth * 0.5, (h / 2) - settingsHeight * 1.25, settingsWidth, settingsHeight);
        button.doLayout(closeX - settingsWidth * 0.5, (h / 2 ) + (settingsHeight / 4), settingsWidth, settingsHeight);
    }
}
