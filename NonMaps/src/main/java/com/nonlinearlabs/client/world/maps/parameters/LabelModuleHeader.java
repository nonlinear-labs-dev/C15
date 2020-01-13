package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.parameters.PlayControls.SourcesAndAmounts.SourcesAndAmounts;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.ContextMenuItem;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class LabelModuleHeader extends LabelSmall {

	public class ParameterGroupContextMenu extends ContextMenu {
		public ParameterGroupContextMenu(OverlayLayout parent) {
			super(parent);

			boolean isLocked = isLocked();

			addChild(new ContextMenuItem(this, (isLocked ? "Unlock" : "Lock") + " this Group") {
				@Override
				public Control click(Position eventPoint) {
					toggleLock();
					return super.click(eventPoint);
				}
			});

			boolean areAllParametersLocked = EditBufferPresenterProvider.getPresenter().allParametersLocked;
			boolean isAnyParameterLocked = EditBufferPresenterProvider.getPresenter().isAnyParameterLocked;

			if (!areAllParametersLocked) {
				addChild(new ContextMenuItem(this, "Lock all Groups") {
					@Override
					public Control click(Position eventPoint) {
						lockAll();
						return super.click(eventPoint);
					}
				});
			}

			if (isAnyParameterLocked) {
				addChild(new ContextMenuItem(this, "Unlock all Groups") {
					@Override
					public Control click(Position eventPoint) {
						unlockAll();
						return super.click(eventPoint);
					}
				});
			}
		}
	}

	public LabelModuleHeader(ParameterGroup parent) {
		super(parent);
	}

	@Override
	public ParameterGroup getParent() {
		return (ParameterGroup) super.getParent();
	}

	@Override
	protected String getDisplayText() {
		return EditBufferModel.get().getAnyGroup(getParent().getName()).longName.getValue();
	}

	protected boolean isLocked() {
		return getParent().isLocked();
	}

	protected void unlockAll() {
		NonMaps.get().getServerProxy().unlockAllGroups();
	}

	protected void lockAll() {
		NonMaps.get().getServerProxy().lockAllGroups();
	}

	protected void toggleLock() {
		NonMaps.get().getServerProxy().toggleGroupLock(getParent().getName());
	}

	public LabelModuleHeader(SourcesAndAmounts parent, String text) {
		super(parent, text);
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		setNonSize(parentsWidthFromFirstPass, getNonPosition().getDimension().getHeight());
	}

	public void drawBackground(Context2d ctx, Rect pixRect) {
		pixRect.drawRoundedRect(ctx, Rect.ROUNDING_TOP, toXPixels(6), toXPixels(2), new Gray(87), null);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect pixRect = getPixRect();
		drawBackground(ctx, pixRect);
		super.draw(ctx, invalidationMask);

		if (isLocked())
			ctx.fillText(getLockSymbol(),
					pixRect.getRight() - ctx.measureText(getLockSymbol()).getWidth() * getLockSymbolOffsetFactor(),
					pixRect.getCenterPoint().getY() + toYPixels(moveFontVerticallyBy()));
	}

	protected String getLockSymbol() {
		return "\ue20A";
	}

	protected float getLockSymbolOffsetFactor() {
		return (float) 1;
	}

	@Override
	protected double getFontHeight() {
		return 16;
	}

	@Override
	protected double getBasicWidth() {
		return 70;
	}

	@Override
	protected double getBasicHeight() {
		return 28;
	}

	@Override
	public RGB getColorFont() {
		return new RGBA(getFontColor(), 0.9);
	}

	private RGB getFontColor() {
		return super.getColorSliderHighlight();
	}

	@Override
	protected boolean shouldShowTextForLevelOfDetail(double levelOfDetail) {
		return true;
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 1;
	}

	public void resetScaling() {
		NonMaps.get().getServerProxy().resetScaling();
	}

	@Override
	public Control onContextMenu(Position pos) {
		boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

		if (showContextMenus) {
			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
			return o.setContextMenu(pos, new ParameterGroupContextMenu(o));
		}
		return super.onContextMenu(pos);
	}
}
