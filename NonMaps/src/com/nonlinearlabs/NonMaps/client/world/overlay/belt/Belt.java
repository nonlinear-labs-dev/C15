package com.nonlinearlabs.NonMaps.client.world.overlay.belt;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.NonMaps.client.Animator;
import com.nonlinearlabs.NonMaps.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.IPreset;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter;
import com.nonlinearlabs.NonMaps.client.world.overlay.DragProxy;
import com.nonlinearlabs.NonMaps.client.world.overlay.Overlay;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.parameters.BeltParameterLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.BeltPresetLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.sound.BeltSoundLayout;

public class Belt extends OverlayLayout {

	private BeltParameterLayout parameterLayout;
	private BeltPresetLayout presetLayout;
	private BeltSoundLayout soundLayout;
	private Animator animator;

	public enum BeltTab {
		Parameter, Sound, Preset
	}

	public Belt(Overlay parent, NonMaps nonMaps) {
		super(parent);
		addChild(soundLayout = new BeltSoundLayout(this));
		addChild(parameterLayout = new BeltParameterLayout(this));
		addChild(presetLayout = new BeltPresetLayout(this));
		setParameterView(false);
	}

	public void openTab(BeltTab tab) {
		switch (tab) {
		case Parameter:
			parameterLayout.fadeIn();
			presetLayout.fadeOut();
			soundLayout.fadeOut();
			break;
		case Sound:
			soundLayout.fadeIn();
			parameterLayout.fadeOut();
			presetLayout.fadeOut();
			break;
		case Preset:
			soundLayout.fadeOut();
			parameterLayout.fadeOut();
			presetLayout.fadeIn();
			break;
		}
	}

	public Parameter getSelectedParameter() {
		return getNonMaps().getNonLinearWorld().getParameterEditor().getSelectedOrSome();
	}

	@Override
	public Overlay getParent() {
		return (Overlay) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		double height = h * (currentHeight - 1);
		super.doLayout(x, y + h - height, w, height);

		soundLayout.doLayout(0, 0, getRelativePosition().getWidth(), h);
		parameterLayout.doLayout(0, 0, getRelativePosition().getWidth(), h);
		presetLayout.doLayout(0, 0, getRelativePosition().getWidth(), h);
	}

	@Override
	public RGB getColorModuleBackground() {
		return new RGB(51, 51, 51);
	}

	public void toggle() {
		if (desiredHeight == 1) {
			desiredHeight = 2;
		} else {
			desiredHeight = 1;
			presetLayout.storeSelectOff();
		}

		if (animator != null)
			animator.cancel();

		animator = new Animator(200);
		animator.addSubAnimation(currentHeight, desiredHeight, new Client() {

			@Override
			public void animate(double v) {
				currentHeight = v;
				requestLayout();
			}
		});
	}

	public boolean isHidden() {
		return desiredHeight == 1;
	}

	// height: 1 = hidden, 2 = expanded
	private double currentHeight = 2;
	private double desiredHeight = 2;

	public void update(Node settingsNode, Node editBufferNode, Node presetManagerNode) {
		presetLayout.update(settingsNode, editBufferNode, presetManagerNode);
		soundLayout.update(settingsNode);
	}

	public boolean isPresetView() {
		return presetLayout.getOpacity() != 0.0 && !isHidden();
	}

	public boolean isParameterView() {
		return parameterLayout.getOpacity() != 0.0 && !isHidden();
	}

	public boolean isSoundView() {
		return soundLayout.getOpacity() != 0.0 && !isHidden();
	}

	void setPresetView(boolean toggleIfHidden) {
		if (toggleIfHidden && isHidden())
			toggle();

		parameterLayout.fadeOut();
		soundLayout.fadeOut();
		presetLayout.fadeIn();
	}

	void setParameterView(boolean toggleIfHidden) {
		if (toggleIfHidden && isHidden())
			toggle();

		parameterLayout.fadeIn();
		presetLayout.fadeOut();
		soundLayout.fadeOut();
	}

	void setSoundView(boolean toggleIfHidden) {
		if (toggleIfHidden && isHidden())
			toggle();

		soundLayout.fadeIn();
		parameterLayout.fadeOut();
		presetLayout.fadeOut();
	}

	@Override
	public Control onKey(final KeyDownEvent event) {

		if (event.getNativeKeyCode() == KeyCodes.KEY_P && getNonMaps().getNonLinearWorld().isShiftDown()) {
			if (isPresetView())
				toggle();
			else
				setPresetView(true);
			return this;

		} else if (event.getNativeKeyCode() == KeyCodes.KEY_E && getNonMaps().getNonLinearWorld().isShiftDown()) {
			if (isParameterView())
				toggle();
			else
				setParameterView(true);
			return this;
		} else if (event.getNativeKeyCode() == KeyCodes.KEY_N && getNonMaps().getNonLinearWorld().isShiftDown()) {
			if (isSoundView())
				toggle();
			else
				setSoundView(true);
			return this;
		} else if (event.getNativeKeyCode() == KeyCodes.KEY_D) {
			if (!isPresetView())
				setPresetView(true);

			getPresetLayout().toggleDirectLoad();
			return this;
		} else if (event.getNativeKeyCode() == KeyCodes.KEY_R) {
			getPresetLayout().renameCurrentPreset();
			return this;
		} else if (event.getNativeKeyCode() == KeyCodes.KEY_S && !event.isControlKeyDown()) {
			if (!isPresetView())
				setPresetView(true);

			getPresetLayout().toggleStoreSelect();
			return this;
		} else if (event.getNativeKeyCode() == KeyCodes.KEY_ENTER) {
			return getPresetLayout().handleEnterKey();
		}

		return null;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control doubleClick() {
		return this;
	}

	public BeltParameterLayout getParameterLayout() {
		return parameterLayout;
	}

	public BeltPresetLayout getPresetLayout() {
		return presetLayout;
	}

	public BeltSoundLayout getSoundLayout() {
		return soundLayout;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		return this;
	}

	@Override
	public Control click(Position eventPoint) {
		return this;
	}

	@Override
	public Control longLeftPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control longRightPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control longPress(Position eventPoint) {
		return this;
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		return this;
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof EditBufferDraggingButton || dragProxy.getOrigin() instanceof IPreset)
			return this;

		return super.drag(pos, dragProxy);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof EditBufferDraggingButton || dragProxy.getOrigin() instanceof IPreset)
			return this;

		return super.drop(pos, dragProxy);
	}

	@Override
	public Control onContextMenu(Position pos) {
		return this;
	}

}
