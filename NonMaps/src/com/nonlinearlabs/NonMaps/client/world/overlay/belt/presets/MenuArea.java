package com.nonlinearlabs.NonMaps.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Gray;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.NonMaps.client.world.overlay.BankInfoDialog;
import com.nonlinearlabs.NonMaps.client.world.overlay.Label;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayControl;
import com.nonlinearlabs.NonMaps.client.world.overlay.OverlayLayout;
import com.nonlinearlabs.NonMaps.client.world.overlay.PresetInfoDialog;

class MenuArea extends OverlayLayout {

	private boolean withLabels = true;

	class MenuLabel extends Label {

		String text;

		public MenuLabel(OverlayLayout parent, String t) {
			super(parent);
			text = t;
			setFontHeightInMM(5);
		}

		@Override
		public String getDrawText(Context2d ctx) {
			return text;
		}

		@Override
		protected TextAlign getAlignment() {
			return TextAlign.LEFT;
		}

	}

	class MenuAndInfo extends OverlayLayout {

		MenuLabel label;
		OverlayControl btn;
		OverlayControl info;
		OverlayControl search;

		public MenuAndInfo(OverlayLayout parent, MenuLabel label, OverlayControl btn, OverlayControl info, OverlayControl search) {
			super(parent);

			this.label = label;
			this.btn = btn;
			this.info = info;
			this.search = search;

			addChild(label);
			addChild(btn);
			addChild(info);

			if (search != null)
				addChild(search);
		}

		public void doLayout(double x, double y, double h, boolean withLabels) {
			double margin = getMargin();
			double buttonWidth = getSmallButtonWidth();
			double w = getDesiredWidth(withLabels);
			double left = 0;

			if (withLabels) {
				label.doLayout(0, 0, getLabelWidth(), h);
				left += label.getRelativePosition().getWidth() + margin;
			} else {
				label.doLayout(0, 0, 0, 0);
			}

			btn.doLayout(left, 0, buttonWidth, h);
			left += buttonWidth + margin;

			info.doLayout(left, 0, buttonWidth, h);
			left += buttonWidth + margin;

			if (search != null)
				search.doLayout(left, 0, buttonWidth, h);

			super.doLayout(x, y, w, h);
		}

		private double getDesiredWidth(boolean withLabels) {
			double buttonWidth = getSmallButtonWidth();
			double margin = getMargin();
			double buttonsWidth = buttonWidth * 3 + margin * 2;
			double labelWidth = getLabelWidth() + margin;
			return buttonsWidth + (withLabels ? labelWidth : 0);
		}
	}

	private MenuAndInfo presets;
	private MenuAndInfo banks;

	private PresetManager getPresetManager() {
		return NonMaps.get().getNonLinearWorld().getPresetManager();
	}

	boolean hasBank() {
		PresetManager pm = getPresetManager();
		String bankUUID = pm.getSelectedBank();

		if (bankUUID != null) {
			return pm.findBank(bankUUID) != null;
		}
		return false;
	}

	boolean hasPreset() {
		PresetManager pm = getPresetManager();
		String bankUUID = pm.getSelectedBank();

		if (bankUUID != null) {
			Bank bank = pm.findBank(bankUUID);
			if (bank != null) {
				String presetUUID = bank.getPresetList().getSelectedPreset();
				return bank.getPresetList().findPreset(presetUUID) != null;
			}
		}
		return false;
	}

	MenuArea(BeltPresetLayout parent) {
		super(parent);

		presets = addChild(new MenuAndInfo(this, new MenuLabel(this, "Preset"), new MenuAreaPresetButton(this),
				new MenuAreaInfoButton(this) {

					@Override
					public void toggle() {
						PresetInfoDialog.toggle();
					}

					@Override
					public int getSelectedPhase() {
						if (hasPreset())
							return PresetInfoDialog.isShown() ? drawStates.active.ordinal() : drawStates.normal.ordinal();
						else
							return drawStates.disabled.ordinal();
					}
				}, new MenuAreaSearchButton(this)));

		banks = addChild(new MenuAndInfo(this, new MenuLabel(this, "Bank"), new MenuAreaBankButton(this), new MenuAreaInfoButton(this) {

			@Override
			public void toggle() {
				if (hasBank())
					BankInfoDialog.toggle();
			}

			@Override
			public int getSelectedPhase() {
				if (hasBank())
					return BankInfoDialog.isShown() ? drawStates.active.ordinal() : drawStates.normal.ordinal();
				else
					return drawStates.disabled.ordinal();
			}
		}, null));
	}

	@Override
	public BeltPresetLayout getParent() {
		return (BeltPresetLayout) super.getParent();
	}

	public void doLayout(double x, double y, double h, boolean withLabels) {
		this.withLabels = withLabels;
		double margin = getMargin();
		presets.doLayout(margin, 0, h / 2, withLabels);
		banks.doLayout(margin, h / 2, h / 2, withLabels);
		double contentWidth = Math.max(presets.getRelativePosition().getWidth(), banks.getRelativePosition().getWidth());
		super.doLayout(x, y, margin + margin + contentWidth, h);
	}

	public boolean isSmall() {
		return !withLabels;
	}

	private double getSmallButtonWidth() {
		return Millimeter.toPixels(11);
	}

	public double getMargin() {
		return getSmallButtonWidth() / 3;
	}

	public double getLabelWidth() {
		return Millimeter.toPixels(15);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		getPixRect().drawRoundedArea(ctx, Millimeter.toPixels(2), Millimeter.toPixels(1), new Gray(16), null);
		super.draw(ctx, invalidationMask);
	}
}
