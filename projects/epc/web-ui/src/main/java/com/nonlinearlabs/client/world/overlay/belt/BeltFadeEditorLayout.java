package com.nonlinearlabs.client.world.overlay.belt;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.belt.Belt.BeltTab;
import com.nonlinearlabs.client.presenters.*;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;

public class BeltFadeEditorLayout extends OverlayLayout {

    private class KeyBed extends SVGImage {
        public KeyBed(Control parent) {
            super(parent, "Keys-Fade-Range.svg");
        }

        @Override
        public int getSelectedPhase() {
            return 0;
        }

        @Override
        public void draw(Context2d ctx, int invalidationMask) {
            super.draw(ctx, invalidationMask);
            if(EditBufferModel.get().soundType.getValue() == SoundType.Layer) {
                drawLayer(ctx, invalidationMask);
            } else if(EditBufferModel.get().soundType.getValue() == SoundType.Split) {
                drawSplit(ctx, invalidationMask);
            }
        }

        public void drawSplit(Context2d ctx, int invalidationMask) {
            RGB cI = new RGB(0x26, 0xb0, 0xff);
            RGBA cIFill = new RGBA(cI, 0.5);
            RGB cII = new RGB(0xff, 0x99, 0x33); 
            RGBA cIIFill = new RGBA(cII, 0.5); 
            
            Rect pix = getPixRect().copy();

            double splitI = pix.getWidth() * getSplitPoint(VoiceGroup.I);
            double splitII = pix.getWidth() * (1.0 - getSplitPoint(VoiceGroup.II));

            ctx.beginPath();
            ctx.setFillStyle(cIFill.toString());
            ctx.setStrokeStyle(cI.toString());
            ctx.moveTo(pix.getLeft(), pix.getTop());
            ctx.lineTo(splitI, pix.getTop());
            ctx.lineTo(splitI, pix.getBottom());
            ctx.lineTo(pix.getLeft(), pix.getBottom());
            ctx.lineTo(pix.getLeft(), pix.getTop());
            ctx.fill();
            ctx.stroke();

            ctx.beginPath();
            ctx.setFillStyle(cIIFill.toString());
            ctx.setStrokeStyle(cII.toString());
            ctx.moveTo(pix.getRight(), pix.getTop());
            ctx.lineTo(pix.getRight(), pix.getBottom());
            ctx.lineTo(splitII, pix.getBottom());
            ctx.lineTo(splitII, pix.getTop());
            ctx.lineTo(pix.getRight(), pix.getTop());
            ctx.fill();
            ctx.stroke();
        }

        public void drawLayer(Context2d ctx, int invalidationMask) {
            
            RGB cI = new RGB(0x26, 0xb0, 0xff);
            RGBA cIFill = new RGBA(cI, 0.5);
            RGB cII = new RGB(0xff, 0x99, 0x33); 
            RGBA cIIFill = new RGBA(cII, 0.5); 
            
            Rect pix = getPixRect().copy();
            
            double fadeFromI = pix.getWidth() * getFadeFrom(VoiceGroup.I); 
            double fadeRangeI = pix.getWidth() * getFadeRange(VoiceGroup.I);

            double fadeFromII = pix.getWidth() * (1.0 - getFadeFrom(VoiceGroup.II)); 
            double fadeRangeII = pix.getWidth() * getFadeRange(VoiceGroup.II);

            ctx.beginPath();
            ctx.setFillStyle(cIFill.toString());
            ctx.setStrokeStyle(cI.toString());
            ctx.moveTo(pix.getLeft(), pix.getTop());
            ctx.lineTo(pix.getLeft() + fadeFromI, pix.getTop());
            double right = Math.min(pix.getLeft() + fadeFromI + fadeRangeI, pix.getRight());
            ctx.lineTo(right, pix.getBottom());
            ctx.lineTo(pix.getLeft(), pix.getBottom());
            ctx.lineTo(pix.getLeft(), pix.getTop());
            ctx.fill();
            ctx.stroke();

            ctx.beginPath();
            ctx.setFillStyle(cIIFill.toString());
            ctx.setStrokeStyle(cII.toString());
            ctx.moveTo(pix.getRight(), pix.getTop());
            ctx.lineTo(pix.getRight() - fadeFromII, pix.getTop());
            double left = Math.max(pix.getRight() - fadeFromII - fadeRangeII, pix.getLeft());
            ctx.lineTo(left, pix.getBottom());
            ctx.lineTo(pix.getRight(), pix.getBottom());
            ctx.lineTo(pix.getRight(), pix.getTop());
            ctx.fill();
            ctx.stroke();
        }

        private double getFadeFrom(VoiceGroup vg) {
            double v = EditBufferModel.get().getParameter(new ParameterId(396, vg)).value.value.getValue();
            return Math.min(1.0, Math.max(v, 0));
        }

        private double getFadeRange(VoiceGroup vg) {
            double v =  EditBufferModel.get().getParameter(new ParameterId(397, vg)).value.value.getValue();
            return Math.min(1.0, Math.max(v, 0));
        }

        private double getSplitPoint(VoiceGroup vg) {
            double v = EditBufferModel.get().getParameter(new ParameterId(356, vg)).value.value.getValue();
            return Math.min(1.0, Math.max(v, 0));
        }
    }

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

    private class ValueDisplay extends Label {

        protected ParameterPresenter presenter;

        ValueDisplay(OverlayLayout parent, ParameterId parameterID) {
            super(parent);

            ParameterPresenterProviders.get().register(parameterID, p -> {
                presenter = p;
                return true;
            });
        }

        protected boolean isBiPolar() {
            return presenter.bipolar;
        }

        public double getValue() {
            return presenter.controlPosition;
        }

        public String getDisplayValue() {
            return presenter.displayValues[presenter.displayValues.length - 1];
        }

        @Override
        public String getDrawText(Context2d ctx) {
            return getDisplayValue();
        }
    }

    private KeyBed keys;
    private PartFadeToggle toggle;
    private ValueDisplay fadePointI;
    private ValueDisplay fadePointII;

    protected BeltFadeEditorLayout(Control parent) {
        super(parent);

        keys = addChild(new KeyBed(this));
        toggle = addChild(new PartFadeToggle(this));
        fadePointI = addChild(new ValueDisplay(this, new ParameterId(396, VoiceGroup.I)));
        fadePointII = addChild(new ValueDisplay(this, new ParameterId(396, VoiceGroup.II)));
        
        EditBufferModel.get().soundType.onChange(type -> {
            installType(type);       
            return true;
        });

    }

    @Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);
	}

    private void installType(SoundType type) {
        if(type == SoundType.Split) {

        } else if(type == SoundType.Layer) {

        }
    }

    @Override
    public void doLayout(double x, double y, double w, double h) {
        super.doLayout(x, y, w, h);
        double parts = 10;
        double partWidth = w / parts;
        double keysW = partWidth * 6;
        double keysLeft = x + partWidth * 2;
        keys.doLayout(x + (w - keys.getPictureWidth()) / 2, y + (keys.getPictureHeight() / 2), keys.getPictureWidth(), keys.getPictureHeight());
        
        double valueHeigth = h / 4;
        fadePointI.doLayout(keysLeft - partWidth, y + valueHeigth * 1.5, partWidth, valueHeigth);
        fadePointII.doLayout(keysLeft + keysW  - partWidth, y + valueHeigth * 1.5, partWidth, valueHeigth);

        double pw = toggle.getPictureWidth();
        double ph = toggle.getPictureHeight();
        toggle.doLayout(w - partWidth, h / 2 - (ph / 2), pw, ph);
    }
}
