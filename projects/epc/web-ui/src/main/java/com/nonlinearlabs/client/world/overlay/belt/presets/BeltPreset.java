package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.presetManager.Preset;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class BeltPreset extends OverlayLayout {

	public enum DropPosition {
		NONE, TOP, MIDDLE, BOTTOM
	}

	private String uuid;
	private OverlayControl color;
	private PresetNumber number;
	private PresetName name;
	private TypeLabel type;
	private DropPosition dropPosition = DropPosition.NONE;

	protected BeltPreset(PresetList parent) {
		super(parent);

		color = addChild(new PresetColorTag(this));
		number = addChild(new PresetNumber(this));
		name = addChild(new PresetName(this));
		type = addChild(new TypeLabel(this));
		name.setFontHeightInMM(4.5);
		number.setFontHeightInMM(4.5);
	}

	@Override
	public PresetList getParent() {
		return (PresetList) super.getParent();
	}

	public String getUuid() {
		return uuid;
	}

	public void setOrigin(String uuid) {
		if (this.uuid != uuid) {
			this.uuid = uuid;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			if (type != null)
				type.bruteForce();

			requestLayout();
		}
		// TODO
		// if (hasCustomPresetSelection())
		{

			invalidate(INVALIDATION_FLAG_UI_CHANGED);

			if (type != null) {
				type.bruteForce();
				type.invalidate(INVALIDATION_FLAG_UI_CHANGED);
			}
		}
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		double numberWidth = Millimeter.toPixels(10);
		double typeWidth = Millimeter.toPixels(5);
		double xSpace = Millimeter.toPixels(5);

		color.doLayout(2, 0, 7, h);
		number.doLayout(7, 0, numberWidth, h);

		double nameWidth = w - (numberWidth + xSpace + typeWidth);
		name.doLayout(numberWidth + xSpace, 0, nameWidth, h);
		type.doLayout(numberWidth + xSpace + nameWidth, 0, typeWidth, h);
	}

	private boolean isSelected() {
		// TODO
		return false;
	}

	private boolean isLoaded() {
		// TODO
		return false;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		// TODO
		/*
		 * boolean loaded = isLoaded() && !mapsPreset.isInStoreSelectMode(); boolean
		 * selected = isSelected() || mapsPreset.isContextMenuActiveOnMe(); boolean
		 * isOriginalPreset = false;
		 * 
		 * if (hasCustomPresetSelection()) isOriginalPreset =
		 * getCustomPresetSelection().isOriginalPreset(mapsPreset.getUUID());
		 * 
		 * RGB colorFill = new RGB(25, 25, 25);
		 * 
		 * if (selected && !isOriginalPreset) colorFill = new RGB(77, 77, 77);
		 * 
		 * if (loaded || isOriginalPreset) colorFill = RGB.blue();
		 * 
		 * getPixRect().fill(ctx, colorFill);
		 */
		super.draw(ctx, overlay, invalidationMask);
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		// TODO
		/*
		 * if (hasCustomPresetSelection()) { if
		 * (getCustomPresetSelection().getSelectedPreset() == mapsPreset.getUUID()) {
		 * mapsPreset.load(); } else {
		 * getCustomPresetSelection().setSelectedPreset(mapsPreset.getUUID()); } return
		 * this; }
		 * 
		 * if (mapsPreset.isSelected()) mapsPreset.load(); else
		 * mapsPreset.selectPreset();
		 * 
		 * getParent().scheduleAutoScroll(PresetList.ScrollRequest.Smooth);
		 */
		return this;
	}

	public DropPosition getDropPosition() {
		return dropPosition;
	}

	@Override
	public Control startDragging(Position pos) {
		return getNonMaps().getNonLinearWorld().getViewport().getOverlay().createDragProxy(this);
	}

	@Override
	public Control drag(Position pos, DragProxy dragProxy) {
		if (!getPixRect().contains(pos))
			return null;

		// TODO
		// if (dragProxy.getOrigin() instanceof IPreset || dragProxy.getOrigin()
		// instanceof EditBufferDraggingButton
		// || dragProxy.getOrigin() instanceof IBank) {
		// Rect r = getPixRect();
		// if (pos.getY() < r.getTop() + r.getHeight() * 0.25)
		// setDropPosition(DropPosition.TOP);
		// else if (pos.getY() < r.getTop() + r.getHeight() * 0.75)
		// setDropPosition(DropPosition.MIDDLE);
		// else
		// setDropPosition(DropPosition.BOTTOM);

		// return this;
		// }
		return super.drag(pos, dragProxy);
	}

	@Override
	public void dragLeave() {
		setDropPosition(DropPosition.NONE);
		super.dragLeave();
	}

	private void setDropPosition(DropPosition p) {
		if (dropPosition != p) {
			dropPosition = p;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {

		// TODO
		// if (dragProxy.getOrigin() instanceof IPreset)
		// insertPreset(dragProxy);
		// else if (dragProxy.getOrigin() instanceof EditBufferDraggingButton)
		// insertEditBuffer();
		// else if (dragProxy.getOrigin() instanceof IBank)
		// insertBank((IBank) dragProxy.getOrigin());

		setDropPosition(DropPosition.NONE);
		return this;
	}

	protected void insertPreset(DragProxy dragProxy) {
		// TODO
		/*
		 * Preset p = mapsPreset; IPreset newPreset = (IPreset) dragProxy.getOrigin();
		 * 
		 * boolean isMove =
		 * p.getParent().getPresetList().findPreset(newPreset.getUUID()) != null;
		 * 
		 * if (isMove) movePreset(p, newPreset); else copyPreset(p, newPreset);
		 */
	}

	protected void insertBank(String bank) {
		// TODO
		/*
		 * Preset p = mapsPreset;
		 * 
		 * switch (dropPosition) { case TOP:
		 * getNonMaps().getServerProxy().insertBankAbove(bank.getUUID(), p.getUUID());
		 * break;
		 * 
		 * case MIDDLE:
		 * getNonMaps().getServerProxy().overwritePresetWithBank(bank.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * case BOTTOM: getNonMaps().getServerProxy().insertBankBelow(bank.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * default: break; }
		 */
	}

	protected void insertEditBuffer() {
		// TODO
		/*
		 * Preset p = mapsPreset;
		 * 
		 * switch (dropPosition) { case TOP:
		 * getNonMaps().getServerProxy().insertEditBufferAbove(p); break;
		 * 
		 * case MIDDLE:
		 * getNonMaps().getServerProxy().overwritePresetWithEditBuffer(p.getUUID());
		 * break;
		 * 
		 * case BOTTOM: getNonMaps().getServerProxy().insertEditBufferBelow(p); break;
		 * 
		 * default: break; }
		 */
	}

	protected void copyPreset(Preset p, String newPreset) {
		// TODO
		/*
		 * switch (dropPosition) { case TOP:
		 * getNonMaps().getServerProxy().insertPresetCopyAbove(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * case MIDDLE:
		 * getNonMaps().getServerProxy().overwritePresetWith(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * case BOTTOM:
		 * getNonMaps().getServerProxy().insertPresetCopyBelow(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * default: break; }
		 */
	}

	protected void movePreset(Preset p, String newPreset) {
		// TODO
		/*
		 * switch (dropPosition) { case TOP:
		 * getNonMaps().getServerProxy().movePresetAbove(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * case MIDDLE: getNonMaps().getServerProxy().movePresetTo(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * case BOTTOM:
		 * getNonMaps().getServerProxy().movePresetBelow(newPreset.getUUID(),
		 * p.getUUID()); break;
		 * 
		 * default: break; }
		 */
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (PresetManagerPresenterProvider.get().getPresenter().inStoreSelectMode)
			return null;

		Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
		return o.setContextMenu(pos, new PresetContextMenu(o, uuid));

	}
}
