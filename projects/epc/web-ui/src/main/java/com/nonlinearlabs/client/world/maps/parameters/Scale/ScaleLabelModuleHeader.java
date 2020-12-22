package com.nonlinearlabs.client.world.maps.parameters.Scale;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.presenters.EditBufferPresenter;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.parameters.LabelModuleHeader;
import com.nonlinearlabs.client.world.maps.parameters.ParameterGroup;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class ScaleLabelModuleHeader extends LabelModuleHeader {

	public ScaleLabelModuleHeader(ParameterGroup parent) {
		super(parent);
	}

	public class ScaleGroupContextMenu extends ParameterGroupContextMenu {

		public ScaleGroupContextMenu(OverlayLayout parent, Scale scaleGroup) {
			super(parent);

			EditBufferPresenter p = EditBufferPresenterProvider.getPresenter();

			if (p.isAnyScaleOffsetParameterNotDefault) {
				addChild(new ContextMenuItem(this, "Reset") {
					@Override
					public Control click(Position eventPoint) {
						resetScaling();
						return super.click(eventPoint);
					}
				});
			}
		}
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (SetupModel.get().localSettings.contextMenus.isTrue()) {
			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
			return o.setContextMenu(pos, new ScaleGroupContextMenu(o, (Scale) getParent()));
		}
		return super.onContextMenu(pos);
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		super.draw(ctx, overlay, menus, invalidationMask);

		Rect pixRect = getPixRect();

		if (hasChangedOffsets()) {
			double fontHeightInUnits = getFontHeight();
			double fontHeightInPixels = toYPixels(fontHeightInUnits) * 0.595;
			ctx.setFont(fontHeightInPixels + "px " + getFontName());
			ctx.fillText("\ue170", pixRect.getLeft() + toXPixels(15),
					pixRect.getCenterPoint().getY() + toYPixels(moveFontVerticallyBy()));
		}
	}

	private boolean hasChangedOffsets() {
		return ((Scale) getParent()).anyValueNotDefault();
	}

}
