package com.nonlinearlabs.client.world.overlay.belt.fadeeditor;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.presenters.FadeEditorPresenter;
import com.nonlinearlabs.client.presenters.FadeEditorPresenterProvider;
import com.nonlinearlabs.client.presenters.FadeEditorPresenter.KeyRange;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class KeyBed extends SVGImage {

    enum SelectedHandle {
        SplitPointI, SplitPointII, FadePointI, FadePointII, FadeRangeI, FadeRangeII, None
    }

    SelectedHandle selection = SelectedHandle.None;

    FadeEditorPresenter presenter;

    public KeyBed(Control parent) {
        super(parent, "Keys-Fade-Range.svg");

        FadeEditorPresenterProvider.get().onChange(p -> {
            presenter = p;
            invalidate(INVALIDATION_FLAG_UI_CHANGED);
            return true;
        });
    }

    @Override
    public int getSelectedPhase() {
        return 0;
    }

    @Override
    public void draw(Context2d ctx, int invalidationMask) {
        super.draw(ctx, invalidationMask);

        if (EditBufferModel.get().soundType.getValue() == SoundType.Layer) {
            drawLayer(ctx);
        } else if (EditBufferModel.get().soundType.getValue() == SoundType.Split) {
            drawSplit(ctx);
        }
    }

    public double getXPosForNote(int note) {
        int keyW = 13;
        int keyPadding = 1;
        return note * (keyW + keyPadding);
    }

    public double getXPosFadeRange(double noteF) {
        int keyW = 13;
        int keyPadding = 1;
        return noteF * (keyW + keyPadding);
    }

    public double quantizeToNoteBorder(double splitValue, VoiceGroup vg) {
        int totalKeys = 61;

        int keyW = 13;
        int keyPadding = 1;

        int key = (int) (splitValue * totalKeys);

        if (vg == VoiceGroup.II && EditBufferModel.get().soundType.getValue() == SoundType.Split && key >= 60) {
            return (key * keyW) + (key * keyPadding);
        }
        return (key * keyW) + (key * keyPadding) + keyW + keyPadding;
    }

    private boolean mouseIsDown = false;

    private void drawHandle(Context2d ctx, boolean focus, Rect r, RGB stroke) {
        RGB color = stroke;

        if(mouseIsDown && focus)
            color = stroke.brighter(20);

        r.drawRoundedRect(ctx, Rect.ROUNDING_ALL, 2, 1, color, RGB.black());
    }

    private void drawSplitHandle(Context2d ctx, VoiceGroup vg) {
        Rect handle = getSplitPointHandleRect(vg);
        SelectedHandle focus = vg == VoiceGroup.I ? SelectedHandle.SplitPointI : SelectedHandle.SplitPointII;
        drawHandle(ctx, selection == focus, handle, presenter.getStrokeColor(vg));
    }

    private void drawSplitPart(Context2d ctx, VoiceGroup vg) {
        Rect pix = getPixRect().copy();

        ctx.beginPath();
        ctx.setFillStyle(presenter.getFillColor(vg).toString());
        ctx.setStrokeStyle(presenter.getStrokeColor(vg).toString());

        KeyRange range = presenter.getSplitRange(vg);
        double from = getXPosForNote(range.from);
        double to = getXPosForNote(range.to);

        ctx.moveTo(pix.getLeft() + from, pix.getTop());
        ctx.lineTo(pix.getLeft() + to, pix.getTop());
        ctx.lineTo(pix.getLeft() + to, pix.getBottom());
        ctx.lineTo(pix.getLeft() + from, pix.getBottom());
        ctx.lineTo(pix.getLeft() + from, pix.getTop());

        ctx.fill();
        ctx.stroke();
    }

    private VoiceGroup selectedVoiceGroup() {
        return presenter.lastSelectedPart;
    }

    public void drawSplit(Context2d ctx) {
        VoiceGroup vg = selectedVoiceGroup();

        if (vg == VoiceGroup.I) {
            drawSplitPart(ctx, VoiceGroup.II);
            drawSplitPart(ctx, VoiceGroup.I);

            drawSplitHandle(ctx, VoiceGroup.II);
            drawSplitHandle(ctx, VoiceGroup.I);
        } else {
            drawSplitPart(ctx, VoiceGroup.I);
            drawSplitPart(ctx, VoiceGroup.II);

            drawSplitHandle(ctx, VoiceGroup.I);
            drawSplitHandle(ctx, VoiceGroup.II);
        }
    }

    public double getFadePointHandleX(VoiceGroup vg) {
        Rect pix = getPixRect();
        return pix.getLeft() + getXPosForNote(presenter.getFadePointRange(vg).indicator);
    }

    public double getFadeRangeHandleX(VoiceGroup vg) {
        Rect pix = getPixRect();
        return pix.getLeft() + getXPosFadeRange(presenter.getFadeRangePos(vg));
    }

    public Rect getLayerFadePointRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        return new Rect(getFadePointHandleX(vg) - halfSize, pix.getTop() - halfSize, size, size);
    }

    private Rect getSplitPointHandleRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        double x = pix.getLeft() + getXPosForNote(presenter.getSplitRange(vg).indicator);
        return new Rect(x - halfSize, pix.getCenterPoint().getY() - halfSize, size, size);
    }

    private Rect getLayerFadeRangeHandleRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        return new Rect(getFadeRangeHandleX(vg) - halfSize, pix.getBottom() - halfSize, size, size);
    }

    private void drawFadeRangeHandle(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect handle = getLayerFadeRangeHandleRect(vg);
        SelectedHandle focus = vg == VoiceGroup.I ? SelectedHandle.FadeRangeI : SelectedHandle.FadeRangeII;
        drawHandle(ctx, selection == focus, handle, stroke);
    }

    private void drawFadePointHandle(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect handle = getLayerFadePointRect(vg);
        SelectedHandle focus = vg == VoiceGroup.I ? SelectedHandle.FadePointI : SelectedHandle.FadePointII;
        drawHandle(ctx, selection == focus, handle, stroke);
    }

    private void drawLayerPart(Context2d ctx, VoiceGroup vg) {
        Rect pix = getPixRect().copy();

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

    public void drawFadeHandle(Context2d ctx, VoiceGroup[] vgs) {
        for (VoiceGroup vg : vgs) {
            drawFadePointHandle(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
            drawFadeRangeHandle(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
        }
    }

    public void drawLayer(Context2d ctx) {
        if (selectedVoiceGroup() == VoiceGroup.I) {
            drawLayerPart(ctx, VoiceGroup.II);
            drawLayerPart(ctx, VoiceGroup.I);
            drawFadeHandle(ctx, new VoiceGroup[] { VoiceGroup.II, VoiceGroup.I });
        } else {
            drawLayerPart(ctx, VoiceGroup.I);
            drawLayerPart(ctx, VoiceGroup.II);
            drawFadeHandle(ctx, new VoiceGroup[] { VoiceGroup.I, VoiceGroup.II });
        }
    }

    @Override
    public Control drag(Position pos, DragProxy dragProxy) {
        return super.drag(pos, dragProxy);
    }

    void selectControl(SelectedHandle handle) {
        selection = handle;
        invalidate(INVALIDATION_FLAG_UI_CHANGED);
    }

    Control handleMouseDownDragStart(Position pos) {
        VoiceGroup first = EditBufferModel.get().voiceGroup.getValue();
        VoiceGroup other = first == VoiceGroup.I ? VoiceGroup.II : VoiceGroup.I;
        VoiceGroup vgs[] = { first, other };

        SoundType type = EditBufferModel.get().soundType.getValue();

        for (VoiceGroup vg : vgs) {
            if (type == SoundType.Layer) {
                if (getLayerFadePointRect(vg).contains(pos)) {
                    selectControl(vg == VoiceGroup.I ? SelectedHandle.FadePointI : SelectedHandle.FadePointII);
                    return this;
                }

                if (getLayerFadeRangeHandleRect(vg).contains(pos)) {
                    selectControl(vg == VoiceGroup.I ? SelectedHandle.FadeRangeI : SelectedHandle.FadeRangeII);
                    return this;
                }
            } else {
                if (getSplitPointHandleRect(vg).contains(pos)) {
                    selectControl(vg == VoiceGroup.I ? SelectedHandle.SplitPointI : SelectedHandle.SplitPointII);
                    return this;
                }
            }
        }

        selectControl(SelectedHandle.None);
        return null;
    }

    @Override
    public Control startDragging(Position pos) {
        Control c = handleMouseDownDragStart(pos);
        if (c == null) {
            return super.startDragging(pos);
        }
        return c;
    }

    @Override
    public Control click(Position pos) {        
        Control c = handleMouseDownDragStart(pos);
        if (c == null) {
            return super.click(pos);
        }
        return c;
    }

    public double getCPForPosition(Position p) throws Exception {
        Rect pix = getPixRect();
        double xPercent = (p.getX() - pix.getLeft()) / pix.getWidth();

        double fadeI = EditBufferModel.get().getParameter(new ParameterId(396, VoiceGroup.I)).value.value.getValue();
        double fadeII = EditBufferModel.get().getParameter(new ParameterId(396, VoiceGroup.II)).value.value.getValue();

        BasicParameterModel fadeRangeI = EditBufferModel.get().getParameter(new ParameterId(397, VoiceGroup.I));
        BasicParameterModel fadeRangeII = EditBufferModel.get().getParameter(new ParameterId(397, VoiceGroup.II));

        boolean fadeIMin = fadeI <= 0;
        boolean fadeIIMax = fadeII >= 1;

        boolean fine = NonMaps.get().getNonLinearWorld().isShiftDown();

        switch (selection) {
            case FadePointI:
            case FadePointII:
            case SplitPointI:
            case SplitPointII:
                return xPercent;
            case FadeRangeI: {
                if (!fadeIMin) {
                    double useableRange = Math.max(0,
                            Math.min(pix.getWidth() - (pix.getWidth() * fadeI), pix.getWidth()));
                    double usableRangePercent = useableRange / pix.getWidth();
                    double newCp = Math.max(0,
                            Math.min((p.getX() - (pix.getLeft() + (pix.getWidth() * fadeI))) / pix.getWidth(),
                                    usableRangePercent));
                    if(fine) {
                        return newCp;
                    } else {
                        return fadeRangeI.value.getQuantizedAndClipped(newCp, false);
                    }        
                } else {
                    return xPercent;
                }
            }
            case FadeRangeII: {
                if (!fadeIIMax) {
                    double useableRange = Math.max(0, Math.min((pix.getWidth() * fadeII), pix.getWidth()));
                    double usableRangePercent = useableRange / pix.getWidth();
                    double newCp = Math.max(0,
                            Math.min(((pix.getLeft() + (pix.getWidth() * fadeII)) - p.getX()) / pix.getWidth(),
                                    usableRangePercent));
                    if(fine) {
                        return newCp;
                    } else {
                        return fadeRangeII.value.getQuantizedAndClipped(newCp, false);
                    }        
                } else {
                    return 1.0 - xPercent;
                }

            }
            case None:
            default:
                break;

        }

        throw new Exception("out of bounds!");
    }

    public void updateCP(Position p) {
        try {
            double cp = getCPForPosition(p);
            switch (selection) {
                case FadePointI:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(396, VoiceGroup.I), cp, true);
                    break;
                case FadePointII:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(396, VoiceGroup.II), cp, true);
                    break;
                case FadeRangeI:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(397, VoiceGroup.I), cp, true);
                    break;
                case FadeRangeII:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(397, VoiceGroup.II), cp, true);
                    break;
                case SplitPointI:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(356, VoiceGroup.I), cp, true);
                    break;
                case SplitPointII:
                    EditBufferUseCases.get().setParameterValue(new ParameterId(356, VoiceGroup.II), cp, true);
                    break;
                case None:
                default:
                    break;
            }
        } catch (Exception e) {
            // expected!
        }
    }

    @Override
    public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
        if (selection != SelectedHandle.None) {
            updateCP(newPoint);
            return this;
        }
        return super.mouseDrag(oldPoint, newPoint, fine);
    }

    @Override
    public Control mouseDown(Position p) {
        mouseIsDown = true;
        invalidate(INVALIDATION_FLAG_UI_CHANGED);
        return this;
    }

	@Override
	public Control mouseUp(Position eventPoint) {
        mouseIsDown = false;
        invalidate(INVALIDATION_FLAG_UI_CHANGED);
        return this;
    }
   
    @Override
    public void onMouseLost() {
        mouseIsDown = false;
    }
}