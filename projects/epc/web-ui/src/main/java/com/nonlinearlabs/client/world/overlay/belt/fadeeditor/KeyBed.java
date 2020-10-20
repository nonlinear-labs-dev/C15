package com.nonlinearlabs.client.world.overlay.belt.fadeeditor;

import com.google.gwt.canvas.dom.client.Context2d;
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

    enum SelectedAnfasser {
        SplitPointI, SplitPointII, FadePointI, FadePointII, FadeRangeI, FadeRangeII, None
    }

    SelectedAnfasser selection = SelectedAnfasser.None;

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

    private void drawAnfasser(Context2d ctx, boolean focus, Rect r, RGB stroke) {
        r.drawRoundedRect(ctx, Rect.ROUNDING_ALL, 2, 1, stroke, RGB.black());

        if (focus) {
            r.getReducedBy(-4).drawRoundedRect(ctx, Rect.ROUNDING_ALL, 1, 1, null, RGB.black());
            r.getReducedBy(-6).drawRoundedRect(ctx, Rect.ROUNDING_ALL, 3, 3, null, stroke);
            r.getReducedBy(-7).drawRoundedRect(ctx, Rect.ROUNDING_ALL, 3, 1, null, RGB.black());
        }
    }

    private void drawSplitAnfasser(Context2d ctx, VoiceGroup vg) {
        Rect anfasser = getSplitPointAnfasserRect(vg);
        SelectedAnfasser focus = vg == VoiceGroup.I ? SelectedAnfasser.SplitPointI : SelectedAnfasser.SplitPointII;
        drawAnfasser(ctx, selection == focus, anfasser, presenter.getStrokeColor(vg));
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

    public void drawSplit(Context2d ctx) {
        VoiceGroup vg = EditBufferModel.get().voiceGroup.getValue();

        if (vg == VoiceGroup.I) {
            drawSplitPart(ctx, VoiceGroup.II);
            drawSplitPart(ctx, VoiceGroup.I);

            drawSplitAnfasser(ctx, VoiceGroup.II);
            drawSplitAnfasser(ctx, VoiceGroup.I);
        } else {
            drawSplitPart(ctx, VoiceGroup.I);
            drawSplitPart(ctx, VoiceGroup.II);

            drawSplitAnfasser(ctx, VoiceGroup.I);
            drawSplitAnfasser(ctx, VoiceGroup.II);
        }
    }

    public double getFadePointAnfasserX(VoiceGroup vg) {
        Rect pix = getPixRect();
        return pix.getLeft() + getXPosForNote(presenter.getFadePointRange(vg).indicator);
    }

    public double getFadeRangeAnfasserX(VoiceGroup vg) {
        Rect pix = getPixRect();
        return pix.getLeft() + getXPosFadeRange(presenter.getFadeRangePos(vg));
    }

    public Rect getLayerFadePointRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        return new Rect(getFadePointAnfasserX(vg) - halfSize, pix.getTop() - halfSize, size, size);
    }

    private Rect getSplitPointAnfasserRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        double x = pix.getLeft() + getXPosForNote(presenter.getSplitRange(vg).indicator);
        return new Rect(x - halfSize, pix.getCenterPoint().getY() - halfSize, size, size);
    }

    private Rect getLayerFadeRangeAnfasserRect(VoiceGroup vg) {
        double size = 20;
        double halfSize = size / 2;
        Rect pix = getPixRect();
        return new Rect(getFadeRangeAnfasserX(vg) - halfSize, pix.getBottom() - halfSize, size, size);
    }

    private void drawFadeRangeAnfasser(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect anfasser = getLayerFadeRangeAnfasserRect(vg);
        SelectedAnfasser focus = vg == VoiceGroup.I ? SelectedAnfasser.FadeRangeI : SelectedAnfasser.FadeRangeII;
        drawAnfasser(ctx, selection == focus, anfasser, stroke);
    }

    private void drawFadePointAnfasser(Context2d ctx, VoiceGroup vg, RGB stroke, RGBA fill) {
        Rect anfasser = getLayerFadePointRect(vg);
        SelectedAnfasser focus = vg == VoiceGroup.I ? SelectedAnfasser.FadePointI : SelectedAnfasser.FadePointII;
        drawAnfasser(ctx, selection == focus, anfasser, stroke);
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

    public void drawFadeAnfasser(Context2d ctx, VoiceGroup[] vgs) {
        for (VoiceGroup vg : vgs) {
            drawFadePointAnfasser(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
            drawFadeRangeAnfasser(ctx, vg, presenter.getStrokeColor(vg), presenter.getFillColor(vg));
        }
    }

    public void drawLayer(Context2d ctx) {
        if (EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.I) {
            drawLayerPart(ctx, VoiceGroup.II);
            drawLayerPart(ctx, VoiceGroup.I);
            drawFadeAnfasser(ctx, new VoiceGroup[] { VoiceGroup.II, VoiceGroup.I });
        } else {
            drawLayerPart(ctx, VoiceGroup.I);
            drawLayerPart(ctx, VoiceGroup.II);
            drawFadeAnfasser(ctx, new VoiceGroup[] { VoiceGroup.I, VoiceGroup.II });
        }
    }

    @Override
    public Control drag(Position pos, DragProxy dragProxy) {
        return super.drag(pos, dragProxy);
    }

    private boolean isSelectedAnfasserOfVoiceGroup(VoiceGroup vg, SelectedAnfasser anfasser) {
        if (vg == VoiceGroup.I) {
            return anfasser == SelectedAnfasser.FadePointI || anfasser == SelectedAnfasser.FadeRangeI
                    || anfasser == SelectedAnfasser.SplitPointI;
        } else {
            return anfasser == SelectedAnfasser.FadePointII || anfasser == SelectedAnfasser.FadeRangeII
                    || anfasser == SelectedAnfasser.SplitPointII;
        }
    }

    void selectControl(SelectedAnfasser anfasser) {
        if (isSelectedAnfasserOfVoiceGroup(VoiceGroup.I, anfasser)) {
            EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.I);
        } else if (isSelectedAnfasserOfVoiceGroup(VoiceGroup.II, anfasser)) {
            EditBufferUseCases.get().selectVoiceGroup(VoiceGroup.II);
        }
        selection = anfasser;
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
                    selectControl(vg == VoiceGroup.I ? SelectedAnfasser.FadePointI : SelectedAnfasser.FadePointII);
                    return this;
                }

                if (getLayerFadeRangeAnfasserRect(vg).contains(pos)) {
                    selectControl(vg == VoiceGroup.I ? SelectedAnfasser.FadeRangeI : SelectedAnfasser.FadeRangeII);
                    return this;
                }
            } else {
                if (getSplitPointAnfasserRect(vg).contains(pos)) {
                    selectControl(vg == VoiceGroup.I ? SelectedAnfasser.SplitPointI : SelectedAnfasser.SplitPointII);
                    return this;
                }
            }
        }

        selectControl(SelectedAnfasser.None);

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

        switch (selection) {
            case FadePointI:
            case FadePointII:
            case SplitPointI:
            case SplitPointII:
                return xPercent;
            case FadeRangeI: {
                double useableRange = Math.max(0, Math.min(pix.getWidth() - (pix.getWidth() * fadeI), pix.getWidth()));
                double usableRangePercent = useableRange / pix.getWidth();
                return Math.max(0, Math.min((p.getX() - (pix.getLeft() + (pix.getWidth() * fadeI))) / pix.getWidth(),
                        usableRangePercent));
            }
            case FadeRangeII: {
                double useableRange = Math.max(0, Math.min((pix.getWidth() * fadeII), pix.getWidth()));
                double usableRangePercent = useableRange / pix.getWidth();
                return Math.max(0, Math.min(((pix.getLeft() + (pix.getWidth() * fadeII)) - p.getX()) / pix.getWidth(),
                        usableRangePercent));
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
        if (selection != SelectedAnfasser.None) {
            updateCP(newPoint);
            return this;
        }
        return super.mouseDrag(oldPoint, newPoint, fine);
    }
}
