package com.nonlinearlabs.client.world.maps.presets.bank;

import java.util.ArrayList;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.IBank;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.Label;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.client.world.overlay.belt.EditBufferDraggingButton;
import com.nonlinearlabs.client.world.overlay.belt.presets.BankContextMenu;
import com.nonlinearlabs.client.world.pointer.DoubleClickWaiter;

public class Header extends Label {

	private class MapsBankContextMenu extends BankContextMenu {
		private MapsBankContextMenu(OverlayLayout parent, Bank bank) {
			super(parent, bank);
		}

		@Override
		protected boolean hasCollapse() {
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
	private DoubleClickWaiter doubleClickWaiter;

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

		if(getParent().isMidiBank())
			drawMidiSymbol(ctx);
	}

	private void drawMidiSymbol(Context2d ctx) {
		Rect pixRect = getPixRect();

		double fontHeightInUnits = getFontHeight();
		double fontHeightInPixels = toYPixels(fontHeightInUnits);

		TextAlign align = TextAlign.RIGHT;
		ctx.setTextAlign(align);
		ctx.setFillStyle(getColorFont().toString());
		ctx.setFont(fontHeightInPixels + "px " + getFontName());

		Position center = pixRect.getCenterPoint();
		center.setX(pixRect.getRight());
		
		ctx.setTextBaseline(TextBaseline.MIDDLE);
		ctx.fillText("\uE0C1", center.getX(), center.getY() + toYPixels(moveFontVerticallyBy()));
	}

	private void drawTooth(Context2d ctx) {
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

		if (SetupModel.get().localSettings.presetDragDrop.getValue() == BooleanValues.on) {

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

	private RGB getBackgroundColor() {
		RGB r = getParent().getColorBankSelect();

		if (isOpenInContextMenu() || mouseCaptured || getParent().isMouseCaptured())
			r = r.brighter(40);

		return r;
	}

	private boolean isOpenInContextMenu() {
		Bank bank = getParent();

		for (ContextMenu cmenu : NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getContextMenus()) {
			if (cmenu instanceof BankContextMenu) {
				BankContextMenu bMenu = (BankContextMenu) cmenu;
				Bank contextMenuBank = bMenu.getBank();
				if (bank == contextMenuBank)
					return true;
			}
		}
		return false;
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
	public Control mouseDown(Position eventPoint) {
		Tracer.log("down");
		Bank parent = getParent();
		parent.getParent().pushBankOntoTop(parent);
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		Tracer.log("up");
		sendPosChangeToServer = true;
		requestLayout();
		return this;
	}

	@Override
	public Control click(Position point) {
		if (doubleClickWaiter != null)
			doubleClickWaiter.cancel();

		doubleClickWaiter = new DoubleClickWaiter(point, (c) -> {
			Tracer.log("click");
			getParent().getParent().selectBank(getParent().getUUID(), true);
		});

		return this;
	}

	@Override
	public Control doubleClick(Position pos) {
		if (doubleClickWaiter != null)
			doubleClickWaiter.cancel();

		doubleClickWaiter = null;

		Tracer.log("double");
		getParent().toggleMinMax();
		return this;
	}

	@Override
	public double getLeftMargin() {
		return 2;
	}

	@Override
	public double getRightMargin() {
		return 2;
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
	public Control startDragging(Position pos) {
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
		ArrayList<Bank> ret = new ArrayList<>();
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
		return getParent();
	}

	@Override
	protected double getFontHeight() {
		return 20;
	}

	@Override
	protected String getFontName() {
		return "'SSP-LW25'";
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (SetupModel.get().localSettings.presetDragDrop.getValue() == BooleanValues.off)
			return null;

		if (getParent().isDraggingControl())
			return null;

		if (dragProxy.getPixRect().contains(pos) && getPixRect().contains(pos)) {
			if (dragProxy.getOrigin() instanceof Bank) {
				Bank bBank = (Bank) dragProxy.getOrigin();
				if (!bBank.hasSlaves()) {
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

		if (bank.getParent().isInStoreSelectMode())
			return null;

		bank.getParent().pushBankOntoTop(bank);

		boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

		if (showContextMenus) {

			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
			return o.setContextMenu(pos, new MapsBankContextMenu(o, getParent()));
		}
		return super.onContextMenu(pos);
	}
}
