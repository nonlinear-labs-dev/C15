package com.nonlinearlabs.client.world.overlay.belt.presets;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.client.LoadToPartMode;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayControl;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class TypeLabel extends OverlayLayout {

	private class SingleTypeLabel extends Label {

		public SingleTypeLabel(TypeLabel parent) {
			super(parent);
		}

		@Override
		public String getDrawText(Context2d ctx) {
			Preset p = TypeLabel.this.getParent().getMapsPreset();

			if (p != null) {
				switch (p.getType()) {
					case Single:
						return "";
					case Split:
						return "\uE0F0";
					case Layer:
						return "\uE0F3";
				}
			}

			return "";
		}

		protected TextAlign getAlignment() {
			return TextAlign.LEFT;
		}

		@Override
		protected Rect getTextRect() {
			Rect r = super.getTextRect().copy();
			r.applyPadding(0, 0, Millimeter.toPixels(1), 0);
			return r;
		}

	}

	private class DualTypeLabel extends OverlayControl {

		public DualTypeLabel(TypeLabel parent) {
			super(parent);
		}

		private TypeLabel getTypeLabel() {
			return (TypeLabel) super.getParent();
		}

		@Override
		public void draw(Context2d ctx, int invalidationMask) {
			LoadToPartMode mode = getLoadToPart();
			if(mode != null) {
				EditBufferModel ebm = EditBufferModel.get();
				VoiceGroup currentVG = ebm.voiceGroup.getValue();
				Preset thisPreset = getTypeLabel().getParent().getMapsPreset();
				String currentOriginUUID = currentVG == VoiceGroup.I ? ebm.sourceUUIDI.getValue() : ebm.sourceUUIDII.getValue();
				VoiceGroup currentOriginVG = currentVG == VoiceGroup.I ? ebm.sourceVGI.getValue() : ebm.sourceVGII.getValue();
	
				boolean isLoaded = thisPreset.getUUID() == currentOriginUUID;
				boolean iLoaded = isLoaded && currentOriginVG == VoiceGroup.I;
				boolean iiLoaded = isLoaded && currentOriginVG == VoiceGroup.II;
	
				boolean isPresetSelected = mode.getSelectedPreset() == getTypeLabel().getParent().getMapsPreset();
				boolean iSelected = isPresetSelected && mode.getSelectedPart() == VoiceGroup.I;
				boolean iiSelected = isPresetSelected && mode.getSelectedPart() == VoiceGroup.II;
	
				switch (getTypeLabel().getParent().getMapsPreset().getType()) {
					case Single:
					default:
						drawSingle(ctx, iLoaded, iSelected);
						break;
					case Layer:
						drawLayer(ctx, iLoaded, iiLoaded, iSelected, iiSelected);
						break;
					case Split:
						drawSplit(ctx, iLoaded, iiLoaded, iSelected, iiSelected);
						break;
				}
			}
		}

		LoadToPartMode getLoadToPart() {
			return NonMaps.get().getNonLinearWorld().getPresetManager().getLoadToPartMode();
		}

		private void drawLayer(Context2d ctx, boolean iLoaded, boolean iiLoaded, boolean iSelected, boolean iiSelected) {
			Rect pix = getParent().getPixRect().copy();
			double height = pix.getHeight() / 2;
			double width = pix.getWidth();

			String loadedColor = getLoadedPresetPartColor().toString();
			String unloadedColor = getUnloadedPresetPartColor().toString();
			String selectedColor = getSelectionIndicationColor().toString();

			ctx.setFillStyle(iLoaded ? loadedColor : unloadedColor);
			ctx.fillRect(pix.getLeft(), pix.getTop(), width, height);

			ctx.setFillStyle(iiLoaded ? loadedColor : unloadedColor);
			ctx.fillRect(pix.getLeft(), pix.getTop() + height, width, height);

			if(iSelected) {
				ctx.setStrokeStyle(selectedColor);
				ctx.strokeRect(pix.getLeft(), pix.getTop(), width, height);
			} else if(iiSelected) {
				ctx.setStrokeStyle(selectedColor);
				ctx.strokeRect(pix.getLeft(), pix.getTop() + height, width, height);
			}
		}

		private RGB getSelectionIndicationColor() {
			return RGB.white();
		}

		private RGB getLoadedPresetPartColor() {
			return RGB.blue().brighter(5);
		}

		private RGB getUnloadedPresetPartColor() {
			return RGB.lightGray();
		}

		private void drawSingle(Context2d ctx, boolean loaded, boolean selected) {
			Rect pix = getParent().getPixRect().copy();

			ctx.setFillStyle(loaded ? getLoadedPresetPartColor().toString() : getUnloadedPresetPartColor().toString());
			ctx.fillRect(pix.getLeft(), pix.getTop(), pix.getWidth(), pix.getHeight());

			if(selected) {
				ctx.setStrokeStyle(getSelectionIndicationColor().toString());
				ctx.setLineWidth(2);
				ctx.strokeRect(pix.getLeft(), pix.getTop(), pix.getWidth(), pix.getHeight());
			}
		}

		private void drawSplit(Context2d ctx, boolean iLoaded, boolean iiLoaded, boolean iSelected, boolean iiSelected) {

			String loadedColor = getLoadedPresetPartColor().toString();
			String unloadedColor = getUnloadedPresetPartColor().toString();
			String selectedColor = getSelectionIndicationColor().toString();

			Rect pix = getParent().getPixRect().copy();
			double height = pix.getHeight();
			double width = pix.getWidth() / 2;

			ctx.setFillStyle(iLoaded ? loadedColor : unloadedColor);
			ctx.fillRect(pix.getLeft(), pix.getTop(), width, height);

			ctx.setFillStyle(iiLoaded ? loadedColor : unloadedColor);
			ctx.fillRect(pix.getLeft() + width, pix.getTop(), width, height);

			if(iSelected) {
				ctx.setStrokeStyle(selectedColor);
				ctx.strokeRect(pix.getLeft(), pix.getTop(), width, height);
			} else if(iiSelected) {
				ctx.setStrokeStyle(selectedColor);
				ctx.strokeRect(pix.getLeft() + width, pix.getTop(), width, height);
			}
		}
	}

	private SingleTypeLabel singleControl = null;
	private DualTypeLabel dualControl = null;

	public TypeLabel(BeltPreset parent) {
		super(parent);
		bruteForce();

		NonMaps.get().getNonLinearWorld().getPresetManager().onLoadToPartModeToggled((v) -> {
			bruteForce();
			return true;
		});
	}

	private boolean isLoadToPartEnabled() {
		return NonMaps.get().getNonLinearWorld().getPresetManager().isInLoadToPartMode();
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		if (dualControl != null)
			dualControl.draw(ctx, invalidationMask);

		if (singleControl != null)
			singleControl.draw(ctx, invalidationMask);
	}

	public void bruteForce() {
		removeAll();
		dualControl = null;
		singleControl = null;

		if (isLoadToPartEnabled()) {
			addChild(dualControl = new DualTypeLabel(this));
		} else {
			addChild(singleControl = new SingleTypeLabel(this));
		}
	}

	@Override
	public BeltPreset getParent() {
		return (BeltPreset) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);

		if(dualControl != null)
			dualControl.doLayout(0, 0, w, h);

		if(singleControl != null)
			singleControl.doLayout(0, 0, w, h);
	}
}
