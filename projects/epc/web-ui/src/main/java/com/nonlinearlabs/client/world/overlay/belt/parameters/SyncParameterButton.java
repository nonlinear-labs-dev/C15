package com.nonlinearlabs.client.world.overlay.belt.parameters;

import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.presenters.DeviceSettings;

public final class SyncParameterButton extends SVGImage {
    
    private DeviceSettings thePresenter;

    public SyncParameterButton(OverlayLayout parent) {
        super(parent, "Sync_Disabled.svg", "Sync_Enabled.svg");
        
        DeviceSettingsProvider.get().register(presenter -> {
            thePresenter = presenter;
            invalidate(INVALIDATION_FLAG_UI_CHANGED);
            return true;
        });
    }

	@Override
	public int getSelectedPhase() {
		return thePresenter.syncSplitsEnabled ? 1 : 0;
	}

	@Override
	public Control click(Position eventPoint) {
		EditBufferUseCases.get().toggleSyncSplit();
		return this;
    }
    
    @Override
    public Control doubleClick(Position p) {
        return this;
    }
}