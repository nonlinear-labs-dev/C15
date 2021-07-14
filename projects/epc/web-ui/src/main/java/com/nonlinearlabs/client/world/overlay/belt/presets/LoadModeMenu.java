package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.LoadToPartMode.LoadToPartModeData;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class LoadModeMenu extends OverlayLayout {

    private class DirectLoadButton extends SVGImage {
        DirectLoadButton(OverlayLayout parent) {
            super(parent, "Dir-Load_Enabled.svg", "Dir-Load_Active.svg");

            SetupModel.get().systemSettings.directLoad.onChange((v) -> {
                requestLayout();
                return true;
            });
        }

        private boolean isDirectLoadActive() {
            return SetupModel.get().systemSettings.directLoad.getBool();
        }

        @Override
        public int getSelectedPhase() {
            return isDirectLoadActive() ? 1 : 0;
        }
        
        @Override
        public Control click(Position eventPoint) {
            boolean inLoadToPart = NonMaps.get().getNonLinearWorld().getPresetManager().isInLoadToPartMode();
            VoiceGroup current = EditBufferModel.get().voiceGroup.getValue();

            if(inLoadToPart) {
                LoadToPartModeData data = NonMaps.get().getNonLinearWorld().getPresetManager().getLoadToPartMode().getCurrentData();
                EditBufferUseCases.get().toggleDirectLoad(data, current);
            } else {
                EditBufferUseCases.get().toggleDirectLoad(null, current);
            }
            
            return this;
        }
    }


    public static boolean isDualEditBuffer() {
        return EditBufferPresenterProvider.getPresenter().soundType != SoundType.Single;
    }

    private class LoadToPartButton extends SVGImage {
        LoadToPartButton(OverlayLayout parent) {
            super(parent, "Load-to-Part_Disabled.svg", "Load-to-Part_Enabled.svg", "Load-to-Part_Active.svg");
        }

        @Override
        public int getSelectedPhase() {
            if(!isDualEditBuffer()) {
                return 0;
            } else {
                if(isLoadToPartActive()) {
                    return 2;
                } else if(NonMaps.get().getNonLinearWorld().getPresetManager().isEmpty()) {
                    return 0;
                } else {
                    return 1;
                }
            }
        }

        private boolean isLoadToPartActive() {
            return NonMaps.get().getNonLinearWorld().getPresetManager().isInLoadToPartMode();
        }

        @Override
        public Control click(Position eventPosition) {
            if(isDualEditBuffer()) {
                if(isLoadToPartActive()) {
                    NonMaps.get().getNonLinearWorld().getPresetManager().endLoadToPartMode();
                } else {
                    NonMaps.get().getNonLinearWorld().getPresetManager().startLoadToPartMode();
                }
                return this;
            } else {
                return null;
            }
        }
    }

    private DirectLoadButton loadButton;
    private LoadToPartButton partButton;

    LoadModeMenu(OverlayLayout parent) {
        super(parent);

        SetupModel.get().systemSettings.directLoad.onChange((d) -> {
            requestLayout();
            return true;
        });

        EditBufferModel.get().voiceGroup.onChange((vg) -> {
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});

        addChild(loadButton = new DirectLoadButton(this));
        addChild(partButton = new LoadToPartButton(this));
    }

    @Override
    public void doLayout(double x, double y, double w, double h) {
        if(isDualEditBuffer())
            doLayoutDual(x, y, w, h);
        else
            doLayoutSingle(x, y, w, h);
    }

    public void doLayoutDual(double x, double y, double w, double h) {
        double margin = getSmallButtonWidth() / 8;
        double width = w;

        double buttonHeight = getSmallButtonWidth();
        super.doLayout(x, y, w, h);

        loadButton.doLayout(0, h / 2 - buttonHeight, width, buttonHeight);
        partButton.doLayout(0, h / 2 + margin, width, buttonHeight);
    }

    public void doLayoutSingle(double x, double y, double w, double h) {
        double margin = getSmallButtonWidth() / 8;

        double width = w - margin;
        double buttonHeight = getSmallButtonWidth();
        
        super.doLayout(x, y, w, h);
        loadButton.doLayout(0, h / 2 - buttonHeight / 2, width, buttonHeight);
        partButton.doLayout(0, 0, 0, 0);
    }

    public double getDesiredWidth() {
        return getSmallButtonWidth() + Millimeter.toPixels(1) + getSmallButtonWidth();
    }

    public double getSmallButtonWidth() {
        return Millimeter.toPixels(11);
    }
}
