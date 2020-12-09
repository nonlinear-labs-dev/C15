package com.nonlinearlabs.client.world.overlay.belt;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.belt.Belt.BeltTab;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.KeyBed;
import com.nonlinearlabs.client.presenters.FadeEditorPresenter;
import com.nonlinearlabs.client.presenters.FadeEditorPresenterProvider;
import com.nonlinearlabs.client.world.overlay.belt.parameters.SyncParameterButton;
import com.nonlinearlabs.client.world.overlay.belt.sound.ValueEdit;

public class BeltFadeEditorLayout extends OverlayLayout {

    private class PartFadeToggle extends SVGImage {

        public PartFadeToggle(Control parent) {
            super(parent, "Fade-Edit-Btn_Active.svg");
        }

        @Override
        public int getSelectedPhase() {
            return 0;
        }

        @Override
        public Control click(Position p) {
            NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getBelt().openTab(BeltTab.Sound);
            return this;
        }
    }

    private class ValueDisplay extends ValueEdit {

        private VoiceGroup voiceGroup;

        ValueDisplay(OverlayLayout parent, ParameterId id) {
            super(parent, id, false);
            voiceGroup = id.getVoiceGroup();
        }

        @Override
        public RGB getColorFont() {
            return presenter.getStrokeColor(voiceGroup);
        }

        @Override
        public void draw(Context2d ctx, int flags) {
            getPixRect().drawValueEditSliderBackgound(ctx, false, getColorFont());
            super.draw(ctx, flags);
        }
    }

    private class OctaveLabel extends Label {
        private String text;

        OctaveLabel(OverlayLayout p, int index) {
            super(p);
            text = "C" + (index + 1);
            setFontHeightInMM(5);
        }

        @Override
        public String getDrawText(Context2d ctx) {
            return text;
        }
    }

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
            fadePoint.doLayout(0, 0, w, pH);
            fadeRange.doLayout(0, pH / 2 * 3, w, pH);
        }
    }

    private class SplitPartIndicators extends OverlayLayout {
        private VoiceGroup voiceGroup;

        private ValueDisplay split;

        SplitPartIndicators(OverlayLayout p, VoiceGroup vg) {
            super(p);
            voiceGroup = vg;
            split = addChild(new ValueDisplay(this, new ParameterId(356, voiceGroup)));
        }

        @Override
        public void doLayout(double x, double y, double w, double h) {
            super.doLayout(x, y, w, h);

            double pH = h / 4;
            split.doLayout(0, pH / 2, w, pH);
        }
    }

    private KeyBed keys;
    private OctaveLabel[] octaveLabels = new OctaveLabel[6];
    private PartFadeToggle toggle;
    private SyncParameterButton button;

    private OverlayControl leftControls = null;
    private OverlayControl rightControls = null;

    private FadeEditorPresenter presenter;

    protected BeltFadeEditorLayout(Control parent) {
        super(parent);

        presenter = FadeEditorPresenterProvider.get().getValue();

        FadeEditorPresenterProvider.get().onChange(p -> {
            presenter = p;
            return true;
        });

        keys = addChild(new KeyBed(this));
        toggle = addChild(new PartFadeToggle(this));
        button = addChild(new SyncParameterButton(this));

        for (int i = 0; i < 6; i++) {
            octaveLabels[i] = addChild(new OctaveLabel(this, i));
        }

        EditBufferModel.get().soundType.onChange(type -> {
            installType(type);
            return true;
        });
    }

    private void installType(SoundType type) {
        
        if (leftControls != null || rightControls != null) {
            removeChild(leftControls);
            removeChild(rightControls);
            leftControls = null;
            rightControls = null;
        }

        button.setVisible(type == SoundType.Split);

        if (type == SoundType.Split) {
            leftControls = addChild(new SplitPartIndicators(this, VoiceGroup.I));
            rightControls = addChild(new SplitPartIndicators(this, VoiceGroup.II));
        } else if (type == SoundType.Layer) {
            leftControls = addChild(new LayerPartIndicators(this, VoiceGroup.I));
            rightControls = addChild(new LayerPartIndicators(this, VoiceGroup.II));
        } else if (type == SoundType.Single) {
            Overlay o = NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
            if (o != null && o.getBelt().isFadeView()) {
                o.getBelt().openTab(BeltTab.Sound);
            }
        }

        requestLayout();
    }

    @Override
    public void doLayout(double x, double y, double w, double h) {
        super.doLayout(x, y, w, h);

        int parts = 8;
        double partWidth = w / parts;

        double baseLine = y + (keys.getPictureHeight() / 2);
        keys.doLayout((w - keys.getPictureWidth()) / 2, (keys.getPictureHeight() / 2), keys.getPictureWidth(),
                keys.getPictureHeight());

        for (int i = 0; i < 6; i++) {
            octaveLabels[i].doLayout(
                    keys.getRelativePosition().getLeft()
                            + keys.quantizeToNoteBorder(i * 12.0 * (1.0 / 61.0), VoiceGroup.Global) - 30,
                    keys.getPictureHeight() * 1.6, 50, 40);
        }

        double toggleW = toggle.getPictureWidth();
        double toggleH = toggle.getPictureHeight();

        button.doLayout(7 * partWidth - toggleW / 2, h / 2 - toggleH / 2, toggleW, toggleH);
        toggle.doLayout(7 * partWidth - toggleW / 2 + toggleW * 1.5, h / 2 - toggleH / 2, toggleW, toggleH);

        double valueWidth = partWidth * 0.75;

        if (leftControls != null)
            leftControls.doLayout(keys.getRelativePosition().getLeft() - valueWidth * 1.25, baseLine, valueWidth, h);

        if (rightControls != null)
            rightControls.doLayout(keys.getRelativePosition().getRight() + valueWidth * 0.25, baseLine, valueWidth, h);
    }

    // Handle overlapping Handles inside KeyBed
    @Override
    public Control startDragging(Position pos) {
        Control c = keys.startDragging(pos);
        if (c == null)
            return super.startDragging(pos);
        return c;
    }

    @Override
    public Control click(Position pos) {
        Control c = keys.click(pos);
        if (c == null)
            return super.click(pos);
        return c;
    }

    @Override
    public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
        Control c = keys.mouseDrag(oldPoint, newPoint, fine);
        if (c == null)
            return super.mouseDrag(oldPoint, newPoint, fine);
        return c;
    }
}
