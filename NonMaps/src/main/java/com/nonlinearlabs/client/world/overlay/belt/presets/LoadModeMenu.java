package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.EditBufferPresenter;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenter;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;

public class LoadModeMenu extends SVGImage {

    LoadModeMenu(OverlayLayout parent) {
        super(parent, "Load_Menu_Disabled.svg", "Load_Menu_Load_To_I.svg", "Load_Menu_Load_To_II.svg", "Load_Menu_Select.svg", "Load_Menu_DL.svg");
    }

    public boolean isInStoreSelectMode() {
        return NonMaps.get().getNonLinearWorld().getPresetManager().isInStoreSelectMode();
    }

    @Override
    public void doLayout(double x, double y, double w, double h) {
        double imgHeight = getSelectedImage().getImgHeight();
        y = (h - imgHeight) / 2;
        y += Millimeter.toPixels(3);
        h = imgHeight;
        super.doLayout(x, y, w, h);
    }

    @Override
    public int getSelectedPhase() {
        PresetManagerPresenter p = PresetManagerPresenterProvider.get().getPresenter();
        EditBufferPresenter eb = EditBufferPresenterProvider.getPresenter();

        if (isInStoreSelectMode())
            return 0;
        else if (p.loadToPartActive)
            if(eb.voiceGroupEnum == VoiceGroup.I)
                return 1;
            else
                return 2;
        else if (!p.directLoadActive)
            return 3;
        else if (p.directLoadActive)
            return 4;

        return 0;
    }

    @Override
    public Control mouseDown(Position pos) {
        return openContextMenu(pos);
    }

    @Override
    public Control onContextMenu(Position pos) {
        return openContextMenu(pos);
    }

    private Control openContextMenu(Position pos) {
        Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();

        if (o.getContextMenu() instanceof LoadModeContextMenu) {
            o.removeExistingContextMenus();
            return this;
        }

        pos = getPixRect().getLeftTop();
        LoadModeContextMenu m = new LoadModeContextMenu(o);
        pos.moveBy(0, -m.getDesiredHeight() - 2);
        return o.setContextMenu(pos, m);
    }

}
