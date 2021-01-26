package com.nonlinearlabs.client.world.overlay.belt.fadeeditor;

import com.google.gwt.canvas.dom.client.Context2d;
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
import com.nonlinearlabs.client.world.Rect;

public class SplitPointKeyBed extends KeyBed {

    private IncrementalChanger changer;
    private VoiceGroup lastTouchedVoiceGroup = VoiceGroup.I;

    public SplitPointKeyBed(Control parent) {
        super(parent);
    }

    @Override
    public void draw(Context2d ctx, int invalidationMask) {
        try (ClipContext c = new ClipContext(ctx, this)) {
            super.draw(ctx, invalidationMask);

            VoiceGroup vg = EditBufferModel.get().voiceGroup.getValue();
            VoiceGroup other = (vg == VoiceGroup.I) ? VoiceGroup.II : VoiceGroup.I;

            drawSplitPart(ctx, other);
            drawSplitPart(ctx, vg);

            drawSplitHandle(ctx, other);
            drawSplitHandle(ctx, vg);
        }

        drawOctaveLabels(ctx);
    }

    @Override
    public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
        if (changer != null) {
            changer.changeBy(fine, newPoint.getX() - oldPoint.getX());
            return this;
        }
        return super.mouseDrag(oldPoint, newPoint, fine);
    }

    private void drawSplitPart(Context2d ctx, VoiceGroup vg) {
        Rect pix = getSelectedImage().getPixRect().copy();

        ctx.beginPath();
        ctx.setFillStyle(presenter.getFillColor(vg).toString());
        ctx.setStrokeStyle(presenter.getStrokeColor(vg).toString());

        KeyRange range = presenter.getSplitRange(vg);
        double from = getXPosForNote(range.from);
        double to = getXPosForNote(range.to) + KEY_WIDTH;

        ctx.moveTo(pix.getLeft() + from, pix.getTop());
        ctx.lineTo(pix.getLeft() + to, pix.getTop());
        ctx.lineTo(pix.getLeft() + to, pix.getBottom());
        ctx.lineTo(pix.getLeft() + from, pix.getBottom());
        ctx.lineTo(pix.getLeft() + from, pix.getTop());

        ctx.fill();
        ctx.stroke();
    }

    private void drawSplitHandle(Context2d ctx, VoiceGroup vg) {
        Rect handle = getSplitPointHandleRect(vg);
        boolean active = false;
        if (changer != null)
            active = lastTouchedVoiceGroup == vg;
        drawHandle(ctx, active, handle, presenter.getStrokeColor(vg));
    }

    protected void drawHandle(Context2d ctx, boolean focus, Rect r, RGB stroke) {
        RGB color = focus ? stroke.brighter(20) : stroke;
        r.drawRoundedRect(ctx, Rect.ROUNDING_ALL, 2, 1, color, RGB.black());
    }

    private Rect getSplitPointHandleRect(VoiceGroup vg) {
        double size = HANDLE_SIZE;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        double x = pix.getLeft() + getXPosForNote(presenter.getSplitRange(vg).indicator);

        if (vg == VoiceGroup.I)
            x += KEY_WIDTH + KEY_PADDING;

        boolean sameNote = presenter.getSplitRange(VoiceGroup.I).to == (presenter.getSplitRange(VoiceGroup.II).from
                - 1);
        Rect r = new Rect(x - halfSize, pix.getCenterPoint().getY() - halfSize, size, size);

        if (sameNote) {
            if (vg == VoiceGroup.I)
                r.moveBy(0, -halfSize - 1);
            else
                r.moveBy(0, halfSize + 1);
        }

        return r;
    }

    @Override
    public Control mouseDown(Position pos) {
        VoiceGroup first = EditBufferModel.get().voiceGroup.getValue();
        VoiceGroup other = first == VoiceGroup.I ? VoiceGroup.II : VoiceGroup.I;
        VoiceGroup vgs[] = { first, other };

        for (VoiceGroup vg : vgs) {
            if (getSplitPointHandleRect(vg).contains(pos)) {
                changer = EditBufferUseCases.get().startEditParameterValue(new ParameterId(356, vg), getPictureWidth());
                lastTouchedVoiceGroup = vg;
                invalidate(INVALIDATION_FLAG_UI_CHANGED);
                return this;
            }
        }
        return null;
    }

    @Override
    public Control mouseUp(Position eventPoint) {
        changer = null;
        invalidate(INVALIDATION_FLAG_UI_CHANGED);
        return super.mouseUp(eventPoint);
    }
}
