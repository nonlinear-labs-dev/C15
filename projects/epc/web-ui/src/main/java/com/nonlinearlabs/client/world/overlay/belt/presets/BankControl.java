package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.presenters.BankPresenter;
import com.nonlinearlabs.client.presenters.BankPresenterProviders;
import com.nonlinearlabs.client.presenters.PresetManagerPresenterProvider;
import com.nonlinearlabs.client.useCases.PresetManagerUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class BankControl extends OverlayLayout {

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
		public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
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
	private static double inset = 4;
	private BankPresenter bank;
	private int bankSubscription = 0;

	BankControl(OverlayLayout parent) {
		super(parent);
		addChild(header = new BankHeader(this));
		addChild(presets = new PresetList(this));
		addChild(selRectangle = new PresetSelectionRectangle(this));

		PresetManagerPresenterProvider.get().register(p -> {
			int s = ++bankSubscription;

			BankPresenterProviders.get().register(p.selectedBank, b -> {
				if (s != bankSubscription)
					return false;

				bank = b;
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
				return true;
			});

			return true;
		});
	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		Rect r = getPixRect();
		RGB black = new Gray(0);
		r.drawRoundedArea(ctx, 0, 1, new Gray(102), black);
		r = r.getReducedBy(2 * inset);
		r.drawRoundedArea(ctx, 0, 0, black, black);
		super.draw(ctx, overlay, invalidationMask);
	}

	void showEmptyLabel(boolean bankEmpty) {
		if (bankEmpty && emptyLabel == null) {
			emptyLabel = prependChild(new EmptyBeltPreset(this));
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
		header.doLayout(inset, inset, w - 2 * inset);

		double listTop = header.getRelativePosition().getBottom();

		presets.doLayout(inset, listTop, w - 2 * inset, h - listTop - inset);
		double emptyLabelHeight = (h - listTop) / 3;

		if (emptyLabel != null)
			emptyLabel.doLayout(inset, listTop + presets.getRelativePosition().getHeight() / 2 - emptyLabelHeight / 2,
					w - 2 * inset, emptyLabelHeight);

		selRectangle.doLayout(0, 0, w - 2 * inset, presets.getPixRect().getHeight() / 3);

	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		return super.mouseDrag(oldPoint, newPoint, fine);
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
		var uc = PresetManagerUseCases.get();

		if (uc.hasCustomPresetSelection())
			return this;

		if (amount > 0)
			uc.selectPreviousPreset();
		else if (amount < 0)
			uc.selectNextPreset();

		return this;
	}

	public BankPresenter getBankPresenter() {
		return bank;
	}

}
