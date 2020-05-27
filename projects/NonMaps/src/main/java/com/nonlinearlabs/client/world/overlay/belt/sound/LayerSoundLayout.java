package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenter;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.IPreset;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.ChoosePresetPartDialog;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.SVGImage;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class LayerSoundLayout extends SoundLayout {

	protected LayerSoundLayout(OverlayLayout parent) {
		super(parent);
		setSettings(new LayerSoundSettings(this));
	}

	private class LayerSoundSettings extends OverlayLayout {
		LayerSoundSettings(LayerSoundLayout parent) {
			super(parent);
			addChild(new VoiceGroupSoundSettings(VoiceGroup.I, this));
			addChild(new VoiceGroupSoundSettings(VoiceGroup.II, this));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double parts = 20;
			double unit = h / parts;
			getChildren().get(0).doLayout(0, 1.5 * unit, w, 8 * unit);
			getChildren().get(1).doLayout(0, 10.5 * unit, w, 8 * unit);
		}
	}

	private class VoiceGroupSoundSettings extends OverlayLayout {
		VoiceGroup group;

		VoiceGroupSoundSettings(VoiceGroup g, LayerSoundSettings parent) {
			super(parent);
			group = g;
			addChild(new VoiceGroupLabel(this));
			addChild(new PresetName(this));
			addChild(new Volume(this));
			addChild(new TuneReference(this));
			addChild(new PartMute(this));
		}

		@Override
		public Control click(Position eventPoint) {
			EditBufferUseCases.get().selectVoiceGroup(group);
			return this;
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double margin = Millimeter.toPixels(3);
			double parts = 20;
			double unit = (w - 2 * margin) / parts;

			getChildren().get(0).doLayout(margin * 1.5, margin * 1.2, 1.5 * unit - 2 * margin, h - 2 * margin);
			getChildren().get(1).doLayout(margin + 1.5 * unit + margin, margin, 8.5 * unit - 2 * margin, h - 2 * margin);
			getChildren().get(2).doLayout(margin + 10 * unit + margin, margin, 4.25 * unit - 2 * margin, h - 2 * margin);
			getChildren().get(3).doLayout(margin + 14.25 * unit + margin, margin, 4.25 * unit - 2 * margin, h - 2 * margin);
			getChildren().get(4).doLayout(margin + 18.5 * unit + margin, margin, 1.5 * unit - 2 * margin, h - 2 * margin);
		}

		@Override
		public void draw(Context2d ctx, int invalidationMask) {
			double margin = Millimeter.toPixels(1);

			EditBufferPresenter presenter = EditBufferPresenterProvider.getPresenter();
			RGB bgColor = (group == VoiceGroup.I) ? presenter.voiceGroupI_BackgroundColor
					: presenter.voiceGroupII_BackgroundColor;

			getPixRect().drawRoundedArea(ctx, margin, 1, bgColor, bgColor);

			double contentLeft = getChildren().get(1).getPixRect().getLeft();
			double contentRight = getPixRect().getRight();
			Rect contentRect = new Rect(contentLeft - 2 * margin, getPixRect().getTop() + 1 * margin,
					contentRight - contentLeft + 1 * margin, getPixRect().getHeight() - 2 * margin);
			contentRect.drawRoundedArea(ctx, margin, 1, new Gray(30), new Gray(30));
			super.draw(ctx, invalidationMask);

			if(isDropTarget) {
				getPixRect().getReducedBy(1).drawRoundedArea(ctx, margin, 1, RGBA.transparent(), RGB.red());
			}
		}

		private boolean isDropTarget = false;


		@Override
		public Control drag(Position pos, DragProxy dragProxy) {
			if (!getPixRect().contains(pos))
				return null;
	
			if (dragProxy.getOrigin() instanceof IPreset) {
				setIsDropTarget(true);
				return this;
			}
			return super.drag(pos, dragProxy);
		}
	
		@Override
		public void dragLeave() {
			setIsDropTarget(false);
			super.dragLeave();
		}
	
		private void setIsDropTarget(boolean isDropTarget) {
			if (this.isDropTarget != isDropTarget) {
				this.isDropTarget = isDropTarget;
				invalidate(INVALIDATION_FLAG_UI_CHANGED);
			}
		}

		private ChoosePresetPartDialog choosePresetPart = null;
	
		@Override
		public Control drop(Position pos, DragProxy dragProxy) {
	
			if (dragProxy.getOrigin() instanceof IPreset)
				if(dragProxy.getOrigin() instanceof Preset) {
					Preset p = (Preset)dragProxy.getOrigin();
					if(p.isDual()) {
						choosePresetPart = new ChoosePresetPartDialog(p, group);		
					} else {
						NonMaps.get().getServerProxy().loadPresetPartIntoPart(p.getUUID(), VoiceGroup.I, group);
					}
				}
	
			setIsDropTarget(false);
			return this;
		}

		private class VoiceGroupLabel extends Label {

			VoiceGroupLabel(VoiceGroupSoundSettings parent) {
				super(parent);
			}

			@Override
			public String getDrawText(Context2d ctx) {
				return group == VoiceGroup.I ? "\uE071" : "\uE072";
			}

			@Override
			protected double getFontHeight(Rect pixRect) {
				return pixRect.getHeight();
			}

			@Override
			protected String crop(Context2d ctx, Rect pixRect, String text) {
				return text;
			}

			@Override
			public RGB getColorFont() {
				if (group == VoiceGroup.I)
					return EditBufferPresenterProvider.getPresenter().voiceGroupI_ForegroundColor;
				return EditBufferPresenterProvider.getPresenter().voiceGroupII_ForegroundColor;
			}

		}

		private class PartMute extends SVGImage {

			PartMute(VoiceGroupSoundSettings parent) {
				super(parent, "Mute_Disabled_A.svg", "Mute_Active_A.svg");
			}

			@Override
			public int getSelectedPhase() {
				return isMuted() ? 1 : 0;
			} 

			boolean isMuted() {
				BasicParameterModel param = EditBufferModel.get().getParameter(new ParameterId(395, group));
				return param.value.getQuantizedAndClipped(true) > 0.5;
			}

			boolean isOtherPartMuted() {
				VoiceGroup otherGroup = getOtherGroup();
				BasicParameterModel param = EditBufferModel.get().getParameter(new ParameterId(395, otherGroup));
				return param.value.getQuantizedAndClipped(true) > 0.5;
			}

			private VoiceGroup getOtherGroup() {
				return (group == VoiceGroup.I) ? VoiceGroup.II : VoiceGroup.I;
			}

			@Override
			public Control click(Position eventPoint) {
				if (isMuted())
					EditBufferUseCases.get().unmutePart(group);
				else if (isOtherPartMuted()) {
					EditBufferUseCases.get().setMuteForPartAndUnmuteOther(group);					
				} else {
					EditBufferUseCases.get().mutePart(group);
				}
				return this;
			}
		}

		private class PresetName extends Label {
			PresetName(VoiceGroupSoundSettings parent) {
				super(parent);
			}

			@Override
			public String getDrawText(Context2d ctx) {
				return EditBufferModel.get().getPresetNameOfVoiceGroupWithSuffix(group);
			}

			@Override
			public void draw(Context2d ctx, int invalidationMask) {
				getPixRect().drawRoundedArea(ctx, Millimeter.toPixels(0.5), 1, new Gray(68), new Gray(86));
				super.draw(ctx, invalidationMask);
			}
		}

		private class TuneReference extends ValueEdit {
			TuneReference(VoiceGroupSoundSettings parent) {
				super(parent, new ParameterId(360, group));
			}
		}

		private class Volume extends ValueEdit {
			Volume(VoiceGroupSoundSettings parent) {
				super(parent, new ParameterId(358, group));
			}
		}
	}

}
