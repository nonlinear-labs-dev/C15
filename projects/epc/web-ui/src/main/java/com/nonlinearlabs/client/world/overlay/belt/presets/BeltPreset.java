package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.dom.client.DataTransfer.DropEffect;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDataType;
import com.nonlinearlabs.client.presenters.ClipboardPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.presenters.PresetPresenter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;
import com.nonlinearlabs.client.useCases.BankUseCases;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class BeltPreset extends OverlayLayout {

	public enum DropPosition {
		NONE, TOP, MIDDLE, BOTTOM
	}

	private OverlayControl color;
	private PresetNumber number;
	private PresetName name;
	private TypeLabel type;
	private DropPosition dropPosition = DropPosition.NONE;

	private PresetPresenter presenter = new PresetPresenter();

	protected BeltPreset(PresetList parent) {
		super(parent, true, true);

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
		return presenter.uuid;
	}

	public void setOrigin(String uuid) {
		PresetPresenterProviders.get().register(uuid, v -> {
			presenter = v;
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});
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
		return presenter.selected;
	}

	private boolean isLoaded() {
		return presenter.loaded;
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, Context2d menus, int invalidationMask) {
		var contextMenuOnPreset = false;

		Overlay o = NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
		for (var cm : o.getContextMenus()) {
			if (cm instanceof PresetContextMenu) {
				var pcm = (PresetContextMenu) cm;
				contextMenuOnPreset = pcm.getPreset() == presenter.uuid;
				if (contextMenuOnPreset)
					break;
			}
		}

		var pm = PresetManagerPresenterProvider.get().getValue();
		boolean loaded = isLoaded() && !pm.inStoreSelectMode && !pm.inLoadToPartMode;
		boolean selected = isSelected() || contextMenuOnPreset;
		boolean isOriginalPreset = false;

		// TODO
		// if (pm.inLoadToPartMode || pm.inStoreSelectMode)
		// isOriginalPreset =
		// pm.getCustomPresetSelection().isOriginalPreset(mapsPreset.getUUID());

		RGB colorFill = new RGB(25, 25, 25);

		if (selected && !isOriginalPreset)
			colorFill = new RGB(77, 77, 77);

		if (loaded || isOriginalPreset)
			colorFill = RGB.blue();

		getPixRect().fill(ctx, colorFill);

		super.draw(ctx, overlay, menus, invalidationMask);
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		var pm = PresetManagerPresenterProvider.get().getValue();

		if (pm.inLoadToPartMode || pm.inStoreSelectMode) {
			if (presenter.selected)
				EditBufferUseCases.get().loadPreset(presenter.uuid);
			else
				PresetManagerUseCases.get().toggleMultipleSelection(presenter.uuid);
			return this;
		}

		if (presenter.selected)
			EditBufferUseCases.get().loadPreset(presenter.uuid);
		else
			BankUseCases.get().selectPreset(presenter.uuid);

		getParent().scheduleAutoScroll(PresetList.ScrollRequest.Smooth);
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

		var cp = ClipboardPresenterProvider.get().getValue();

		if (cp.dndType.any(DragDataType.Preset, DragDataType.Bank)) {
			Rect r = getPixRect();
			if (pos.getY() < r.getTop() + r.getHeight() * 0.25)
				setDropPosition(DropPosition.TOP);
			else if (pos.getY() < r.getTop() + r.getHeight() * 0.75)
				setDropPosition(DropPosition.MIDDLE);
			else
				setDropPosition(DropPosition.BOTTOM);

			return this;
		}
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
		DropEffect effect = DropEffect.MOVE;
		switch (dropPosition) {
			case TOP:
				BankUseCases.get().dropAbove(presenter.uuid, effect);
				break;

			case MIDDLE:
				BankUseCases.get().dropOn(presenter.uuid, effect);
				break;

			case BOTTOM:
				BankUseCases.get().dropBelow(presenter.uuid, effect);
				break;

			case NONE:
				break;
		}

		setDropPosition(DropPosition.NONE);
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (PresetManagerPresenterProvider.get().getValue().inStoreSelectMode)
			return null;

		Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
		return o.setContextMenu(pos, new PresetContextMenu(o, presenter.uuid));

	}
}
