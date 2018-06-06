package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.ServerProxy;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Gray;
import com.nonlinearlabs.NonMaps.client.world.IBank;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.RGB;
import com.nonlinearlabs.NonMaps.client.world.RGBA;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.parameters.Parameter.Initiator;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayControl;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.SVGImage;

public class BankControl extends OverlayLayout implements IBank {

	private class PresetSelectionRectangle extends OverlayControl {
		public PresetSelectionRectangle(OverlayLayout parent) {
			super(parent);
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
		}

		public PresetList getPresetList() {
			BankControl b = (BankControl) getParent();
			return b.presets;
		}

		@Override
		public void draw(Context2d ctx, int invalidationMask) {
			Rect presetPixrect = getPresetList().getPixRect().copy();
			Rect r = getRelativePosition().copy();
			r.setTop(presetPixrect.getTop() + presetPixrect.getHeight() / 3);
			r.setLeft(presetPixrect.getLeft());
			r.drawRoundedArea(ctx, 0, NonMaps.mmToPixels(0.1), new RGBA(new RGB(0, 0, 0), 0), new RGB(255, 255, 255));
		}

	}

	private BankHeader header;
	private PresetSelectionRectangle selRectangle;
	private PresetList presets;
	private EmptyBeltPreset emptyLabel;

	BankControl(OverlayLayout parent) {
		super(parent);
		addChild(header = new BankHeader(this));
		addChild(presets = new PresetList(this));
		addChild(selRectangle = new PresetSelectionRectangle(this));
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect r = getPixRect();
		RGB black = new Gray(0);
		double border = getSpaceBetweenChildren();
		r.drawRoundedArea(ctx, border, 1, new Gray(102), black);
		r = r.getReducedBy(8);
		r.drawRoundedArea(ctx, border, 0, black, black);
		super.draw(ctx, invalidationMask);
	}

	public Bank getBankInCharge() {
		PresetManager pm = NonMaps.theMaps.getNonLinearWorld().getPresetManager();
		String selectedBankUuid = pm.getSelectedBank();
		Bank bank = pm.findBank(selectedBankUuid);
		return bank;
	}

	void showEmptyLabel(boolean bankEmpty) {
		if (bankEmpty && emptyLabel == null) {
			emptyLabel = prependChild(new EmptyBeltPreset(this, getBankInCharge()));
		} else if (!bankEmpty && emptyLabel != null) {
			removeChild(emptyLabel);
			emptyLabel = null;
		}
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		if (w < Millimeter.toPixels(30))
			w = 0;

		super.doLayout(x, y, w, h);

		double headerHeight = SVGImage.calcSVGDimensionToPixels(24);
		double border = getSpaceBetweenChildren() * 2;

		header.doLayout(border, border, w - 2 * border, headerHeight);

		double listTop = header.getRelativePosition().getBottom() + getSpaceBetweenChildren();

		presets.doLayout(border, listTop, w - 2 * border, h - listTop - border);

		double emptyLabelHeight = (h - listTop - border) / 3;

		if (emptyLabel != null)
			emptyLabel.doLayout(border / 2, listTop / 2 + emptyLabelHeight / 2, w - 2 * border, emptyLabelHeight);

		selRectangle.doLayout(0, 0, w - 2 * border, presets.getPixRect().getHeight() / 3 - border / 3);
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return super.mouseDrag(oldPoint, newPoint, fine);
	}

	@Override
	public String getUUID() {
		return getBankInCharge().getUUID();
	}

	public void update(Node pmNode) {
		if (pmNode != null) {
			if (ServerProxy.didChange(pmNode)) {
				Bank b = getBankInCharge();
				if (b != null)
					showEmptyLabel(b.getPresetList().getPresetCount() == 0);
				else
					showEmptyLabel(true);
				update();
			}
		}
	}

	public void update() {
		presets.update();
	}

	public void setHeaderTitleFontHeightInMM(int mm) {
		header.setFontHeightInMM(mm);
	}

	public PresetList getPresetList() {
		return presets;

	}

	public double getHorizontalCenterLinePosition() {
		return presets.getHorizontalCenterLinePosition() + getRelativePosition().getTop();
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		PresetManager pm = NonMaps.theMaps.getNonLinearWorld().getPresetManager();

		if (pm.isInStoreSelectMode())
			return this;

		if (amount > 0)
			pm.selectPreviousPreset(Initiator.EXPLICIT_USER_ACTION);
		else if (amount < 0)
			pm.selectNextPreset(Initiator.EXPLICIT_USER_ACTION);

		return this;
	}

	public boolean isInStoreSelectMode() {
		return getBankInCharge().isInStoreSelectMode();
	}

	public void renameCurrentPreset() {
		getPresetList().renameCurrentPreset();
	}
}
