package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.FadeEditorPresenter;
import com.nonlinearlabs.client.presenters.FadeEditorPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.belt.Belt.BeltTab;
import com.nonlinearlabs.client.world.overlay.belt.fadeeditor.KeyBed;
import com.nonlinearlabs.client.world.pointer.Down;
import com.nonlinearlabs.client.world.pointer.Gesture;

class KeyBedEditor extends OverlayLayout {
    protected FadeEditorPresenter presenter;
    protected CloseButton closeButton;

    protected class CloseButton extends SVGImage {

        public CloseButton(Control parent) {
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

    protected class ValueDisplay extends ValueEdit {

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
        public void draw(Context2d ctx, Context2d overlayCtx, int flags) {
            getPixRect().drawValueEditSliderBackgound(ctx, false, getColorFont());
            super.draw(ctx, overlayCtx, flags);
        }
    }

    class HorizonatlScrollPane extends OverlayLayout {
        double scrolling = 0;
        KeyBed nested;

        HorizonatlScrollPane(OverlayLayout parent, KeyBed nested) {
            super(parent);
            this.nested = addChild(nested);
        }

        @Override
        public void doLayout(double x, double y, double w, double h) {
            super.doLayout(x, y, w, h);
            getChildren().get(0).doLayout(0, 0, this.nested.getPictureWidth(), h);
            setScroll(scrolling);
        }

        void setScroll(double a) {
            double max = getRelativePosition().getWidth() - nested.getPictureWidth();
            a = Math.max(max, Math.min(0, a));

            if (a != scrolling) {
                scrolling = a;
                invalidate(INVALIDATION_FLAG_SCROLLED);
            }
        }

        @Override
        public void draw(Context2d ctx, Context2d overlayCtx, int invalidationMask) {
            try (ClipContext c = new ClipContext(ctx, this)) {
                ctx.translate(scrolling, 0);
                super.draw(ctx, overlayCtx, invalidationMask);
            }
        }

        @Override
        public Control mouseDown(Position eventPoint) {
            return this;
        }

        @Override
        public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
            Dimension dim = newPoint.getVector(oldPoint);
            setScroll(scrolling + dim.getWidth());
            return this;
        }

        @Override
        public Control handleGesture(Gesture g) {
            if (g instanceof Down) {
                Down d = (Down) g;
                d.getPosition().moveBy(-scrolling, 0);
                Control ret = super.handleGesture(g);
                d.getPosition().moveBy(scrolling, 0);
                return ret;
            }
            return super.handleGesture(g);
        }
    }

    protected KeyBedEditor(Control parent) {
        super(parent);

        closeButton = addChild(new CloseButton(this));

        FadeEditorPresenterProvider.get().onChange(p -> {
            presenter = p;
            invalidate(INVALIDATION_FLAG_UI_CHANGED);
            return true;
        });

    }

}