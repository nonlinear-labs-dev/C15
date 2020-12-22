package com.nonlinearlabs.client.world.overlay.belt.fadeeditor;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.FadeEditorPresenter.KeyRange;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.IncrementalChanger;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;

public class FadePointKeyBed extends KeyBed {

    enum SelectedHandle {
        FadePointI, FadePointII, FadeRangeI, FadeRangeII, None
    }

    private IncrementalChanger changer;
    private ParameterId selectedParameter;

    public FadePointKeyBed(Control parent) {
        super(parent);
    }

    @Override
    public void draw(Context2d ctx, Context2d overlayCtx, Context2d menus, int invalidationMask) {
        try (ClipContext c = new ClipContext(ctx, this)) {
            super.draw(ctx, overlayCtx, menus, invalidationMask);

            if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.I) {
                drawLayerPart(ctx, VoiceGroup.II);
                drawLayerPart(ctx, VoiceGroup.I);
                drawFadeHandle(ctx, new VoiceGroup[] { VoiceGroup.II, VoiceGroup.I });
            } else {
                drawLayerPart(ctx, VoiceGroup.I);
                drawLayerPart(ctx, VoiceGroup.II);
                drawFadeHandle(ctx, new VoiceGroup[] { VoiceGroup.I, VoiceGroup.II });
            }
        }

        drawOctaveLabels(ctx);
    }

    private void drawLayerPart(Context2d ctx, VoiceGroup vg) {
        Rect pix = getSelectedImage().getPixRect().copy();

        ctx.beginPath();
        ctx.setFillStyle(presenter.getFillColor(vg).toString());
        ctx.setStrokeStyle(presenter.getStrokeColor(vg).toString());

        KeyRange range = presenter.getFadePointRange(vg);
        double from = getXPosForNote(range.from);
        double to = getXPosForNote(range.to);
        double toBottom = getXPosFadeRange(presenter.getFadeRangePos(vg));

        if (vg == VoiceGroup.I) {
            ctx.moveTo(pix.getLeft() + from, pix.getTop());
            ctx.lineTo(pix.getLeft() + to, pix.getTop());
            ctx.lineTo(pix.getLeft() + toBottom, pix.getBottom());
            ctx.lineTo(pix.getLeft() + to, pix.getBottom());
            ctx.lineTo(pix.getLeft() + from, pix.getBottom());
            ctx.lineTo(pix.getLeft() + from, pix.getTop());
        } else {
            ctx.moveTo(pix.getLeft() + from, pix.getTop());
            ctx.lineTo(pix.getLeft() + toBottom, pix.getBottom());
            ctx.lineTo(pix.getLeft() + to, pix.getBottom());
            ctx.lineTo(pix.getLeft() + to, pix.getTop());
            ctx.lineTo(pix.getLeft() + from, pix.getTop());
        }

        ctx.fill();
        ctx.stroke();
    }

    private void drawFadeRangeHandle(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect handle = getLayerFadeRangeHandleRect(vg);
        boolean sel = selectedParameter != null && selectedParameter.getVoiceGroup() == vg
                && selectedParameter.getNumber() == 397;
        drawBottomHandle(ctx, sel, handle, stroke);
    }

    private void drawFadePointHandle(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect handle = getLayerFadePointRect(vg);
        boolean sel = selectedParameter != null && selectedParameter.getVoiceGroup() == vg
                && selectedParameter.getNumber() == 396;
        drawTopHandle(ctx, sel, handle, stroke);
    }

    public void drawFadeHandle(Context2d ctx, VoiceGroup[] vgs) {
        for (VoiceGroup vg : vgs) {
            drawFadePointHandle(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
            drawFadeRangeHandle(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
        }
    }

    protected void drawTopHandle(Context2d ctx, boolean focus, Rect r, RGB stroke) {
        RGB color = focus ? stroke.brighter(20) : stroke;
        double corner = Millimeter.toPixels(1);
        Position center = r.getCenterPoint();
        ctx.beginPath();
        ctx.moveTo(r.getLeft(), center.getY());
        ctx.lineTo(r.getLeft(), r.getTop() + corner);
        ctx.arcTo(r.getLeft(), r.getTop(), r.getLeft() + corner, r.getTop(), corner);
        ctx.lineTo(r.getRight() - corner, r.getTop());
        ctx.arcTo(r.getRight(), r.getTop(), r.getRight(), r.getTop() + corner, corner);
        ctx.lineTo(r.getRight(), center.getY());
        ctx.lineTo(center.getX(), r.getBottom());
        ctx.lineTo(r.getLeft(), center.getY());
        ctx.closePath();
        ctx.setFillStyle(color.toString());
        ctx.fill();
        ctx.setStrokeStyle(RGB.black().toString());
        ctx.setLineWidth(Millimeter.toPixels(0.5));
        ctx.stroke();
    }

    protected void drawBottomHandle(Context2d ctx, boolean focus, Rect r, RGB stroke) {
        RGB color = focus ? stroke.brighter(20) : stroke;
        double corner = Millimeter.toPixels(1);
        Position center = r.getCenterPoint();
        ctx.beginPath();
        ctx.moveTo(r.getLeft(), center.getY());
        ctx.lineTo(center.getX(), r.getTop());
        ctx.lineTo(r.getRight(), center.getY());
        ctx.lineTo(r.getRight(), r.getBottom() - corner);
        ctx.arcTo(r.getRight(), r.getBottom(), r.getRight() - corner, r.getBottom(), corner);
        ctx.lineTo(r.getLeft() + corner, r.getBottom());
        ctx.arcTo(r.getLeft(), r.getBottom(), r.getLeft(), r.getBottom() - corner, corner);
        ctx.closePath();
        ctx.setFillStyle(color.toString());
        ctx.fill();
        ctx.setStrokeStyle(RGB.black().toString());
        ctx.setLineWidth(Millimeter.toPixels(0.5));
        ctx.stroke();
    }

    public double getFadePointHandleX(VoiceGroup vg) {
        Rect pix = getSelectedImage().getPixRect();
        return pix.getLeft() + getXPosForNote(presenter.getFadePointRange(vg).indicator);
    }

    public double getFadeRangeHandleX(VoiceGroup vg) {
        Rect pix = getSelectedImage().getPixRect();
        return pix.getLeft() + getXPosFadeRange(presenter.getFadeRangePos(vg));
    }

    public Rect getLayerFadePointRect(VoiceGroup vg) {
        double size = HANDLE_SIZE;
        double halfSize = size / 2;
        Rect pix = getSelectedImage().getPixRect();
        return new Rect(getFadePointHandleX(vg) - halfSize, pix.getTop() - halfSize, size, size);
    }

    private Rect getLayerFadeRangeHandleRect(VoiceGroup vg) {
        double size = HANDLE_SIZE;
        double halfSize = size / 2;
        Rect pix = getSelectedImage().getPixRect();
        return new Rect(getFadeRangeHandleX(vg) - halfSize, pix.getBottom() + halfSize - size, size, size);
    }

    @Override
    public Control mouseDown(Position pos) {
        VoiceGroup first = EditBufferModel.get().voiceGroup.getValue();
        VoiceGroup other = first == VoiceGroup.I ? VoiceGroup.II : VoiceGroup.I;
        VoiceGroup vgs[] = { first, other };

        for (VoiceGroup vg : vgs) {
            if (getLayerFadePointRect(vg).contains(pos)) {
                selectedParameter = new ParameterId(396, vg);
                changer = EditBufferUseCases.get().startEditParameterValue(selectedParameter, getPictureWidth());
                invalidate(INVALIDATION_FLAG_UI_CHANGED);
                return this;
            }
            if (getLayerFadeRangeHandleRect(vg).contains(pos)) {
                selectedParameter = new ParameterId(397, vg);
                double direction = vg == VoiceGroup.II ? -getPictureWidth() : getPictureWidth();
                changer = EditBufferUseCases.get().startEditParameterValue(selectedParameter, direction);
                invalidate(INVALIDATION_FLAG_UI_CHANGED);
                return this;
            }
        }
        return null;
    }

    public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
        if (changer != null) {
            changer.changeBy(fine, newPoint.getX() - oldPoint.getX());
            return this;
        }
        return super.mouseDrag(oldPoint, newPoint, fine);
    }

    @Override
    public Control mouseUp(Position eventPoint) {
        changer = null;
        selectedParameter = null;
        invalidate(INVALIDATION_FLAG_UI_CHANGED);
        return super.mouseUp(eventPoint);
    }

}
