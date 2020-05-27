package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.CachingMapsControl;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonRect;

public abstract class ParameterGroup extends LayoutResizingVertical {

	private NonRect m_viewportPosAtClick;
	private String groupName;

	public ParameterGroup(MapsLayout parent, String groupName) {
		super(parent);
		this.groupName = groupName;
	}

	public String getName() {
		return groupName;
	}

	public boolean isLocked() {
		Control c = recurseChildren(new ControlFinder() {
			@Override
			public boolean onWayDownFound(Control ctrl) {
				return (ctrl instanceof Parameter);
			}
		});

		Parameter p = (Parameter) c;
		return p.isLocked();
	}

	@Override
	public <T extends MapsControl> T addChild(T child) {
		if (child instanceof ParameterGroupControls) {
			super.addChild(new CachingMapsControl(this, child));
			return child;
		}

		return super.addChild(child);
	}

	public void fillBackground(Context2d ctx) {
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, toXPixels(6), toXPixels(2), new Gray(26), null);
	}

	public void drawBorder(Context2d ctx) {
		getPixRect().drawRoundedRect(ctx, Rect.ROUNDING_ALL, toXPixels(6), toXPixels(2), null,
				getColorModuleHeaderBackground());
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		fillBackground(ctx);
		super.draw(ctx, invalidationMask);
		drawBorder(ctx);
	}

	@Override
	public Control doubleClick(Position pos) {
		NonRect viewportPosAtClick = NonMaps.theMaps.getNonLinearWorld().getViewport().getNonPosition().copy();

		if (zoomTo(this)) {
			m_viewportPosAtClick = viewportPosAtClick;
		} else if (m_viewportPosAtClick != null) {
			NonMaps.theMaps.getNonLinearWorld().animateViewport(m_viewportPosAtClick, true);
			m_viewportPosAtClick = null;
		}
		getNonMaps().getNonLinearWorld().getPresetManager().resetStoredViewportPosition();
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		return this;
	}
}
