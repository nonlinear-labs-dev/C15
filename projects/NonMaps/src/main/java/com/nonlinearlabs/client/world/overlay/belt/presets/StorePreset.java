package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.RepeatingCommand;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.SVGImagePhase;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;

class StorePreset extends SVGImage {

	public enum Action {
		INSERT, OVERWRITE, APPEND, DEFAULT
	}

	Action action = Action.APPEND;
	RepeatingCommand dragDelay = null;

	class DraggableButtonPhase extends SVGImagePhase implements EditBufferDraggingButton {

		public DraggableButtonPhase(SVGImage parent, String imageName) {
			super(parent, imageName);
		}

	}

	StorePreset(OverlayLayout parent) {
		super(parent, "Store_Enabled.svg", "Store_Active.svg", "Store_Disabled.svg", "Store_Drag.svg");
		super.selectPhase(2);
	}

	@Override
	public int getSelectedPhase() {
		boolean isCapturing = isCaptureControl() && dragDelay == null;
		if (isCapturing || isDraggingControl()) {
			return drawStates.drag.ordinal();
		}

		if (isCaptureControl() && !isDraggingControl())
			return drawStates.active.ordinal();

		return drawStates.normal.ordinal();
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		dragDelay = new RepeatingCommand() {

			@Override
			public boolean execute() {
				if (dragDelay != null) {
					dragDelay = null;
					requestLayout();
				}
				return false;
			}
		};

		Scheduler.get().scheduleFixedDelay(dragDelay, 500);
		return super.mouseDown(eventPoint);
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		dragDelay = null;
		requestLayout();
		return super.mouseUp(eventPoint);
	}

	@Override
	protected SVGImagePhase createPhase(String s) {
		return new DraggableButtonPhase(this, s);
	}

	@Override
	public Control startDragging(Position eventPoint) {
		return getNonMaps().getNonLinearWorld().getViewport().getOverlay().createDragProxy(getPhase(1));
	}

	@Override
	public Control click(Position eventPoint) {
		fire();
		return this;
	}

	private void fire() {
		boolean hasSelectedBank = getPresetManager().hasSelectedBank();

		if (getPresetManager().isInStoreSelectMode())
			hasSelectedBank = getPresetManager().getStoreSelectMode().getSelectedBank() != null;

		if (!hasSelectedBank)
			createNewBank();
		else
			storeToBank();

		getPresetManager().endStoreSelectMode();
	}

	protected PresetManager getPresetManager() {
		return getNonMaps().getNonLinearWorld().getPresetManager();
	}

	public void storeToBank() {
		switch (action) {
			case APPEND:
				if (getPresetManager().isInStoreSelectMode()) {
					getNonMaps().getServerProxy()
							.appendEditBuffer(getPresetManager().getStoreSelectMode().getSelectedBank());
				} else {
					getNonMaps().getServerProxy().appendPreset();
				}
				break;

			case INSERT:
				if (getPresetManager().isInStoreSelectMode()) {
					getNonMaps().getServerProxy()
							.insertPreset(getPresetManager().getStoreSelectMode().getSelectedPreset());
				} else {
					getNonMaps().getServerProxy().insertPreset(getPresetManager().getSelectedPreset());
				}
				break;

			case OVERWRITE:
				if (getPresetManager().isInStoreSelectMode()
						&& getPresetManager().getStoreSelectMode().getSelectedPreset() != null) {
					getNonMaps().getServerProxy()
							.overwritePresetWithEditBuffer(getPresetManager().getStoreSelectMode().getSelectedPreset());
				} else if (getPresetManager().getSelectedPreset() != null) {
					getNonMaps().getServerProxy().overwritePresetWithEditBuffer(getPresetManager().getSelectedPreset());
				} else {
					getNonMaps().getServerProxy().overwritePreset();
				}
				break;
			default:
				break;
		}
	}

	protected void createNewBank() {
		getNonMaps().getServerProxy()
				.newBankFromEditBuffer(getNonMaps().getNonLinearWorld().getNonPosition().getCenterPoint());
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		if (isDraggingControl() && !isVisibilityForced()) {
			drawDetachedState(ctx);
		} else {
			super.draw(ctx, invalidationMask);
		}
	}

	protected void drawDetachedState(Context2d ctx) {
		Rect r = getPixRect().copy();
		double reduced = 0;
		r.reduceHeightBy(reduced);
		r.reduceWidthBy(reduced);
		r.drawRoundedArea(ctx, 2, 1, new Gray(26), new Gray(0));
	}

	@Override
	public Control onKey(KeyDownEvent event) {
		if (event.getNativeKeyCode() == com.google.gwt.event.dom.client.KeyCodes.KEY_S && event.isControlKeyDown()) {
			storeToBank();
			return this;
		}
		return super.onKey(event);
	}

	public void update(Node settingsNode, Node presetManagerNode) {
		if (ServerProxy.didChange(settingsNode)) {
			String str = ServerProxy.getChildText(settingsNode, "PresetStoreModeSetting", "value");
			if (str != null && !str.isEmpty()) {
				switch (str.toLowerCase()) {
					case "append":
						action = Action.APPEND;
						break;

					case "insert":
						action = Action.INSERT;
						break;

					case "overwrite":
						action = Action.OVERWRITE;
						break;
				}

				invalidate(INVALIDATION_FLAG_UI_CHANGED);
			}
		}
	}

	public Control handleEnterKey() {
		fire();
		return this;
	}

}
