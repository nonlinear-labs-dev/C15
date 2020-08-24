package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;

class LayerDisplay extends OverlayLayout {

    public LayerDisplay(OverlayLayout parent) {
        super(parent);

        EditBufferModel.get().soundType.onChange(v -> {
            setVisible(v != SoundType.Single);
            return true;
        });

        NonMaps.get().getNonLinearWorld().getPresetManager().onLoadToPartModeToggled((x) -> {
            invalidate(INVALIDATION_FLAG_UI_CHANGED);
            return true;
        });
    }

    public boolean isLoadIntoPartEnabled() {
        return NonMaps.get().getNonLinearWorld().getPresetManager().isInLoadToPartMode();
    }

    @Override
    public void draw(Context2d ctx, int invalidationMask) {
        double colorFactor = 0.43;
        RGB c = EditBufferPresenterProvider.getPresenter().voiceGroupIndicationColor.adjust(colorFactor);

        RGB stroke = RGBA.black();
        getPixRect().fill(ctx, c);

        switch (EditBufferPresenterProvider.getPresenter().soundType) {
            case Split:
                drawSplitIndication(ctx, c, stroke);
                break;
            case Layer:
                drawLayerIndication(ctx, c, stroke);
                break;
            default:
                break;
        }
    }

    private VoiceGroup getSelectedVoiceGroup() {
        return EditBufferPresenterProvider.getPresenter().voiceGroupEnum;
    }

    private void drawSplitIndication(Context2d ctx, RGB fill, RGB stroke) {
        VoiceGroup selected = getSelectedVoiceGroup();
        Rect pix = getPixRect().copy();

        final double margin = Millimeter.toPixels(1);
        final double partWidth = Millimeter.toPixels(2);
        final double partHeight = Millimeter.toPixels(5);
        final double yMargin = Millimeter.toPixels(2.5);

        final RGB ogFill = fill;
        final RGB lighterFill = ogFill.brighter(96);

        if (selected == VoiceGroup.I)
            fill = lighterFill;

        Rect box = new Rect(0, pix.getTop() + yMargin, partWidth, partHeight);

        box.setLeft(pix.getCenterPoint().getX() - margin / 2 - partWidth);
        box.fillAndStroke(ctx, fill, 2, stroke);

        fill = ogFill;

        if (selected == VoiceGroup.II)
            fill = lighterFill;

        box.setLeft(pix.getCenterPoint().getX() + margin / 2);
        box.fillAndStroke(ctx, fill, 2, stroke);

        double loadtoPartHeight = Math.abs(pix.getBottom() - box.getBottom()) * 0.8;
        double loadtoPartWidth = partWidth * 0.8;
        if (isLoadIntoPartEnabled()) {
            if (selected == VoiceGroup.I) {
                drawTriangleUpwards(ctx, RGB.white(), RGB.black(),
                        new Position(pix.getCenterPoint().getX() - margin / 2 - partWidth / 2, pix.getBottom()),
                        loadtoPartWidth, loadtoPartHeight);
            } else {
                drawTriangleUpwards(ctx, RGB.white(), RGB.black(),
                        new Position(pix.getCenterPoint().getX() + margin / 2 + partWidth / 2, pix.getBottom()),
                        loadtoPartWidth, loadtoPartHeight);
            }
        }
    }

    private void drawLayerIndication(Context2d ctx, RGB fill, RGB stroke) {
        VoiceGroup selected = getSelectedVoiceGroup();
        Rect pix = getPixRect().copy();

        final double margin = Millimeter.toPixels(1);
        final double partWidth = Millimeter.toPixels(5);
        double partHeight = Millimeter.toPixels(2);

        final RGB ogFill = fill;
        final RGB lighterFill = ogFill.brighter(96);

        if (selected == VoiceGroup.I)
            fill = lighterFill;

        Rect box = new Rect(pix.getCenterPoint().getX() - partWidth / 2, 0, partWidth, partHeight);

        final double boxIY = pix.getCenterPoint().getY() - margin / 2 - partHeight;
        box.setTop(boxIY);
        box.fillAndStroke(ctx, fill, 2, stroke);

        fill = ogFill;

        if (selected == VoiceGroup.II)
            fill = lighterFill;

        final double boxIIY = pix.getCenterPoint().getY() + margin / 2;
        box.setTop(boxIIY);
        box.fillAndStroke(ctx, fill, 2, stroke);

        partHeight = partHeight * 0.8;

        if (isLoadIntoPartEnabled()) {
            if (selected == VoiceGroup.I) {
                drawTriangleSideways(ctx, RGB.white(), RGB.black(),
                        new Position(pix.getRight() - 1, boxIY + partHeight / 2), partHeight, partHeight);
            } else {
                drawTriangleSideways(ctx, RGB.white(), RGB.black(),
                        new Position(pix.getRight() - 1, boxIIY + partHeight / 2), partHeight, partHeight);
            }
        }
    }

    private void drawTriangleSideways(Context2d ctx, RGB fill, RGB stroke, Position pos, double width, double height) {
        ctx.beginPath();
        ctx.moveTo(pos.getX(), pos.getY() - width);
        ctx.lineTo(pos.getX(), pos.getY() + width);
        ctx.lineTo(pos.getX() - height, pos.getY());
        ctx.closePath();
        ctx.setFillStyle(fill.toString());
        ctx.setLineWidth(1);
        ctx.setStrokeStyle(stroke.toString());
        ctx.fill();
        ctx.stroke();
    }

    private void drawTriangleUpwards(Context2d ctx, RGB fillColor, RGB strokeColor, Position pos, double width,
            double height) {
        ctx.beginPath();
        ctx.moveTo(pos.getX() + width, pos.getY());
        ctx.lineTo(pos.getX() - width, pos.getY());
        ctx.lineTo(pos.getX(), pos.getY() - height);
        ctx.closePath();
        ctx.setFillStyle(fillColor.toString());
        ctx.setLineWidth(1);
        ctx.setStrokeStyle(strokeColor.toString());
        ctx.fill();
        ctx.stroke();
    }

    @Override
    public Control mouseDown(Position eventPoint) {
        if (isVisible())
            return this;
        return super.mouseDown(eventPoint);
    }

    @Override
    public Control click(Position eventPoint) {
        if (isVisible()) {
            EditBufferUseCases.get().toggleVoiceGroup();
            
            if(SetupModel.get().systemSettings.syncVoiceGroups.isTrue()) {
                NonMaps.theMaps.getServerProxy().syncVoiceGroup();
            }
            
            return this;
        }

        return super.click(eventPoint);
    }

    @Override
    public Control doubleClick(Position pos) {
        return this;
    }

    
}