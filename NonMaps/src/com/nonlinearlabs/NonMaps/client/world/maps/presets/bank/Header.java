package com.nonlinearlabs.NonMaps.client.world.maps.presets.bank;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.contextStates.ClipContext;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.IBank;
import com.nonlinearlabs.NonMaps.client.world.IPreset;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.RGBA;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.Label;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.NonMaps.client.world.overlay.DragProxy;
import com.nonlinearlabs.NonMaps.client.world.overlay.Overlay;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.EditBufferDraggingButton;
import com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets.BankContextMenu;
import com.nonlinearlabs.NonMaps.client.world.overlay.setup.ContextMenusSetting;

public class Header extends Label {

	private class MapsBankContextMenu extends BankContextMenu {
		private MapsBankContextMenu(OverlayLayout parent, Bank bank) {
			super(parent, bank);
		}

		@Override
		protected boolean hasMinimize() {
			return true;
		}

		@Override
		protected boolean hasPaste() {
			return true;
		}

		@Override
		protected boolean hasBankCreationRights() {
			return false;
		}
	}

	private boolean sendPosChangeToServer = false;
	private boolean mouseCaptured = false;
	private boolean isDropTarget = false;
	private String bankName = "";

	public Header(Bank parent) {
		super(parent, "");
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect r = getPixRect();
		r.drawRoundedRect(ctx, Rect.ROUNDING_TOP, toXPixels(6), toXPixels(1), getBackgroundColor(), null);

		super.draw(ctx, invalidationMask);

		if (isDropTarget) {
			drawTooth(ctx);
		}
	}

	protected void drawTooth(Context2d ctx) {
		try (ClipContext clipper = new ClipContext(ctx, this)) {
			drawClipped(ctx);
		}
	}

	protected void drawClipped(Context2d ctx) {
		Rect r = getPixRect();
		ctx.setFillStyle(new RGBA(255, 0, 0, 0.5).toString());

		double dim = r.getHeight() / 2;
		for (double x = r.getLeft(); x < r.getRight(); x += dim)
			drawTeeth(ctx, x, r.getCenterPoint().getY() - dim / 2, dim);
	}

	private void drawTeeth(Context2d ctx, double x, double y, double dim) {
		ctx.beginPath();
		ctx.moveTo(x, y);
		ctx.lineTo(x + dim, y);
		ctx.lineTo(x + dim / 2, y + dim);
		ctx.closePath();
		ctx.fill();
	}

	@Override
	public void dragLeave() {
		setIsDropTarget(false);
		super.dragLeave();
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		Bank b = getParent();
		PresetManager pm = b.getParent();

		if (b == dragProxy.getOrigin())
			return null;

		if (getNonMaps().getNonLinearWorld().getViewport().getOverlay().getSetup().getPresetDragDropSetting().isEnabled()) {
			if (dragProxy.getOrigin() instanceof IPreset) {
				if (pm.hasMultiplePresetSelection()) {
					getNonMaps().getServerProxy().dropPresetsOnBank(pm.getMultiSelection().getCSV(), b);
					pm.getMultiSelection().clear();
				} else {
					getNonMaps().getServerProxy().dropPresetOnBank((IPreset) dragProxy.getOrigin(), b);
				}
			} else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton)
				getNonMaps().getServerProxy().dropEditBufferOnBank(b);
			else if (dragProxy.getOrigin() instanceof IBank) {
				Bank draggedBank = (Bank) dragProxy.getOrigin();
				if (!draggedBank.hasSlaves()) {
					getNonMaps().getServerProxy().dropBankOnBank((IBank) dragProxy.getOrigin(), b);
				}
			}
		}

		setIsDropTarget(false);
		return this;
	}

	public void dropPresets(PresetManager pm, Bank b) {
		for (String uuid : pm.getMultiSelection().getSelectedPresets()) {
			Preset p = pm.findPreset(uuid);
			getNonMaps().getServerProxy().dropPresetOnBank(p, b);
		}
	}

	private RGB getBackgroundColor() {
		RGB r = getParent().getColorBankSelect();

		if (!getWorld().isSpaceDown() && mouseCaptured || getParent().isMouseCaptured())
			r = r.brighter(40);

		return r;
	}

	@Override
	public RGB getColorFont() {
		return new RGB(25, 25, 25);
	}

	@Override
	public Bank getParent() {
		return (Bank) super.getParent();
	}

	@Override
	public boolean setText(String text) {
		return super.setText(text.isEmpty() ? "<untitled bank>" : text);
	}

	@Override
	protected String getDisplayText() {
		return getParent().getOrderNumber() + " - " + bankName;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		if(getWorld().isSpaceDown())
			return this;
		
		sendPosChangeToServer = true;
		requestLayout();
		return super.mouseUp(eventPoint);
	}

	@Override
	public Control click(Position point) {
		if(getWorld().isSpaceDown())
			return this;
		
		getParent().getParent().selectBank(getParent().getUUID(), true);
		return this;
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		super.doFirstLayoutPass(levelOfDetail);

		if (sendPosChangeToServer) {
			sendPosChangeToServer = false;

			Bank parent = getParent();
			getNonMaps().getServerProxy().onBankPositionChanged(parent);
		}
	}

	@Override
	public void doSecondLayoutPass(double parentsWidthFromFirstPass, double parentsHeightFromFirstPass) {
		setNonSize(parentsWidthFromFirstPass, getNonPosition().getHeight());
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if(getWorld().isSpaceDown())
			return this;
		
		Bank parent = getParent();
		parent.getParent().pushBankOntoTop(parent);
		return this;
	}

	@Override
	public Control startDragging(Position pos) {
		if(getWorld().isSpaceDown())
			return this;
		
		Control ret = null;

		for (Bank bank : getBanksThatMoveWhenMovingBank(getParent())) {
			if (bank == getParent()) {
				ret = getNonMaps().getNonLinearWorld().getViewport().getOverlay().addDragProxy(bank);
			} else {
				getNonMaps().getNonLinearWorld().getViewport().getOverlay().addDragProxy(bank);
			}
		}

		getParent().undockBank();

		return ret;
	}

	private ArrayList<Bank> getBanksThatMoveWhenMovingBank(Bank parent) {
		ArrayList<Bank> ret = new ArrayList<Bank>();
		parent.collectCluster(ret);
		return ret;
	}

	public void onMouseLost() {
		mouseCaptured = false;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	public void onMouseCaptured() {
		mouseCaptured = true;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		if(getWorld().isSpaceDown())
			return getWorld().mouseDrag(oldPoint, newPoint, fine);
		
		return getParent();
	}

	@Override
	protected double getFontHeight() {
		return 20;
	}

	@Override
	protected String getFontName() {
		return "nonlinearfont";
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (!getNonMaps().getNonLinearWorld().getViewport().getOverlay().getSetup().getPresetDragDropSetting().isEnabled())
			return null;

		if (getParent().isDraggingControl())
			return null;

		if (dragProxy.getPixRect().contains(pos) && getPixRect().contains(pos)) {
			if (dragProxy.getOrigin() instanceof Bank) {
				Bank bBank = (Bank) dragProxy.getOrigin();
				if (bBank.hasSlaves() == false) {
					setIsDropTarget(true);
					return this;
				}
			} else if (dragProxy.getOrigin() instanceof IPreset) {
				setIsDropTarget(true);
				return this;
			} else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton) {
				setIsDropTarget(true);
				return this;
			}
		}

		return super.drag(pos, dragProxy);
	}

	@Override
	public int getDragRating(Position newPoint, DragProxy dragProxy) {
		if (getPixRect().contains(newPoint))
			return super.getDragRating(newPoint, dragProxy) * 100;
		return 0;
	}

	private void setIsDropTarget(boolean isDropTarget) {
		if (this.isDropTarget != isDropTarget) {
			this.isDropTarget = isDropTarget;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	public void setBankName(String name) {
		bankName = name;
	}

	public String getBankName() {
		return bankName;
	}

	@Override
	public Control onContextMenu(Position pos) {
		Bank bank = getParent();

		if (bank.isInStoreSelectMode())
			return null;

		bank.getParent().pushBankOntoTop(bank);

		ContextMenusSetting contextMenuSettings = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getSetup()
				.getContextMenuSettings();
		if (contextMenuSettings.isEnabled()) {
			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
			return o.setContextMenu(pos, new MapsBankContextMenu(o, getParent()));
		}
		return super.onContextMenu(pos);
	}
}
