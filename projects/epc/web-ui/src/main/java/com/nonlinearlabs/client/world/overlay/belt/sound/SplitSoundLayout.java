package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Renameable;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
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
import com.nonlinearlabs.client.world.RenameDialog;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.ChoosePresetPartDialog;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class SplitSoundLayout extends SoundLayout {

	protected SplitSoundLayout(OverlayLayout parent) {
		super(parent);
		setSettings(new SplitSoundSettings(this));
	}

	private class SplitSoundSettings extends OverlayLayout {
		SplitSoundSettings(SplitSoundLayout parent) {
			super(parent);
			addChild(new SplitFXIndicator(VoiceGroup.I, this));
			addChild(new VoiceGroupSoundSettings(VoiceGroup.I, this));
			addChild(new SplitPoint(this));
			addChild(new VoiceGroupSoundSettings(VoiceGroup.II, this));
			addChild(new SplitFXIndicator(VoiceGroup.II, this));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double margin = Millimeter.toPixels(2);
			double splitPointHeight = Math.min(h, Millimeter.toPixels(30));
			double splitPointWidth = Math.min(w / 4, Millimeter.toPixels(40));
			double settingWidth = (w - splitPointWidth - 2 * margin) / 2;

			getChildren().get(1).doLayout(0, margin, settingWidth, h - 2 * margin);
			getChildren().get(2).doLayout(settingWidth + margin, (h - splitPointHeight) / 2, splitPointWidth,
					splitPointHeight);
			getChildren().get(3).doLayout(w - settingWidth, margin, settingWidth, h - 2 * margin);

			SplitFXIndicator fxI = (SplitFXIndicator) getChildren().get(0);
			SplitFXIndicator fxII = (SplitFXIndicator) getChildren().get(4);

			double fxIW = fxI.getSelectedImage().getImgWidth();
			double fxIH = fxI.getSelectedImage().getImgHeight();

			double fxIIW = fxII.getSelectedImage().getImgWidth();
			double fxIIH = fxII.getSelectedImage().getImgHeight();

			fxI.doLayout(-fxIW, h / 2 - (fxIH / 2), fxIW, fxIH);
			fxII.doLayout(w, h / 2 - (fxIIH / 2), fxIIW, fxIIH);
		}

		@Override
		public void draw(Context2d ctx, int flags) {
			if (SetupModel.get().systemSettings.syncSplit.isFalse()) {
				SplitPoint splits = (SplitPoint) getChildren().get(2);
				Rect left = splits.getRectOfVG(VoiceGroup.I);
				Rect right = splits.getRectOfVG(VoiceGroup.II);
				Rect startLeft = getChildren().get(1).getPixRect();
				Rect endRight = getChildren().get(3).getPixRect();
				RGB bgI = EditBufferPresenterProvider.getPresenter().voiceGroupI_BackgroundColor;
				RGB bgII = EditBufferPresenterProvider.getPresenter().voiceGroupII_BackgroundColor;

				ctx.setFillStyle(bgI.toString());
				ctx.fillRect(startLeft.getRight(), left.getCenterPoint().getY() - 2,
						left.getLeft() - startLeft.getRight(), 4);

				ctx.setFillStyle(bgII.toString());
				ctx.fillRect(right.getRight(), right.getCenterPoint().getY() - 2, endRight.getLeft() - right.getRight(),
						4);

				if(!EditBufferPresenterProvider.getPresenter().splitOverlap) {
					RGB color = EditBufferModel.get().voiceGroup.getValue() == VoiceGroup.I ? bgI : bgII;
					ctx.setFillStyle(color.toString());
					double startY = left.getCenterPoint().getY();
					double endY = right.getCenterPoint().getY();
					ctx.fillRect(left.getCenterPoint().getX() - 2, startY, 4, endY - startY);
				}
			}


			super.draw(ctx, flags);
		}
	}

	private class SplitPoint extends OverlayLayout {

		private boolean syncEnabled = false;

		public SplitPoint(OverlayLayout parent) {
			super(parent);

			SetupModel.get().systemSettings.syncSplit.onChange(enabled -> {
				onSettingChanged(enabled.equals(BooleanValues.on));
				return true;
			});

		}

		public Rect getRectOfVG(VoiceGroup vg) {
			if (!syncEnabled) {
				if (vg == VoiceGroup.I) {
					return getChildren().get(1).getPixRect();
				} else {
					return getChildren().get(2).getPixRect();
				}
			} else {
				return getChildren().get(1).getPixRect();
			}
		}

		private void onSettingChanged(boolean isSyncEnabled) {
			syncEnabled = isSyncEnabled;

			removeAll();

			if (isSyncEnabled) {
				addChild(new SplitPointLabel(this, "Split Point"));
				addChild(new SplitPointValue(this));
			} else {
				addChild(new SplitPointLabel(this, "Split Points"));
				addChild(new SplitPointValue(this, VoiceGroup.I));
				addChild(new SplitPointValue(this, VoiceGroup.II));
			}

			requestLayout();
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double quarterHeight = h / 4;

			if (syncEnabled) {
				getChildren().get(0).doLayout(0, 0, w, quarterHeight * 0.75);
				getChildren().get(1).doLayout(0, quarterHeight * 1.5, w, quarterHeight);
			} else {
				getChildren().get(0).doLayout(0, 0, w, quarterHeight * 0.75);
				getChildren().get(1).doLayout(0, quarterHeight, w, quarterHeight);
				getChildren().get(2).doLayout(0, quarterHeight * 2.4, w, quarterHeight);
			}
		}

		private class SplitPointLabel extends Label {

			private String text;

			public SplitPointLabel(OverlayLayout parent, String text) {
				super(parent);
				this.text = text;
			}

			@Override
			public String getDrawText(Context2d ctx) {
				return text;
			}
		}

		private class SplitPointValue extends ValueEdit {

			public SplitPointValue(OverlayLayout parent) {
				super(parent, new ParameterId(356, EditBufferModel.get().voiceGroup.getValue()), true);
			}

			public SplitPointValue(OverlayLayout parent, VoiceGroup vg) {
				super(parent, new ParameterId(356, vg), false);
			}
		}
	}

	private class VoiceGroupSoundSettings extends OverlayLayout {
		VoiceGroup group;

		VoiceGroupLabel m_voiceGroupLabel;
		PresetName m_presetName;
		VolumeLabel m_volumeLabel;
		Volume m_volumeValue;
		TuneLabel m_tuneLabel;
		TuneReference m_tuneValue;

		VoiceGroupSoundSettings(VoiceGroup g, SplitSoundSettings parent) {
			super(parent);
			group = g;
			m_voiceGroupLabel = addChild(new VoiceGroupLabel(this));
			m_presetName = addChild(new PresetName(this));
			m_volumeLabel = addChild(new VolumeLabel(this));
			m_volumeValue = addChild(new Volume(this));
			m_tuneLabel = addChild(new TuneLabel(this));
			m_tuneValue = addChild(new TuneReference(this));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double margin = Millimeter.toPixels(3);
			double parts = 20;
			double xunit = (w - 2 * margin) / parts;
			double yunit = (h - 2 * margin) / parts;

			double middleLine = h / 2;
			double labelHeight = ((h / 20) * 9) - margin * 2;

			double voiceGroupLabelWidth = 3 * xunit;
			double presetNameWidth = 16 * xunit;
			double parameterLabelWidth = 6 * xunit;
			double parameterValueWidth = 10 * xunit;

			double vgLabelMargin = margin;
			if (xunit < margin) {
				vgLabelMargin /= 2;
			}

			if (group == VoiceGroup.I) {
				m_voiceGroupLabel.doLayout(vgLabelMargin, middleLine - labelHeight / 2, voiceGroupLabelWidth,
						labelHeight);
				m_presetName.doLayout(margin + 4 * xunit, margin, presetNameWidth, 5 * yunit);
				m_volumeLabel.doLayout(margin + 4 * xunit, margin + 8 * yunit, parameterLabelWidth, 5 * yunit);
				m_volumeValue.doLayout(margin + 10 * xunit, margin + 8 * yunit, parameterValueWidth, 5 * yunit);
				m_tuneLabel.doLayout(margin + 4 * xunit, margin + 15 * yunit, parameterLabelWidth, 5 * yunit);
				m_tuneValue.doLayout(margin + 10 * xunit, margin + 15 * yunit, parameterValueWidth, 5 * yunit);
			} else {
				m_tuneValue.doLayout(margin, margin + 15 * yunit, parameterValueWidth, 5 * yunit);
				m_tuneLabel.doLayout(margin + parameterValueWidth, margin + 15 * yunit, parameterLabelWidth, 5 * yunit);
				m_volumeValue.doLayout(margin, margin + 8 * yunit, parameterValueWidth, 5 * yunit);
				m_volumeLabel.doLayout(margin + parameterValueWidth, margin + 8 * yunit, parameterLabelWidth,
						5 * yunit);
				m_presetName.doLayout(margin, margin, presetNameWidth, 5 * yunit);
				m_voiceGroupLabel.doLayout(w - vgLabelMargin - voiceGroupLabelWidth, middleLine - labelHeight / 2,
						voiceGroupLabelWidth, labelHeight);
			}
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
			if (group == VoiceGroup.I) {
				Rect contentRect = new Rect(contentLeft - 2 * margin, getPixRect().getTop() + 1 * margin,
						contentRight - contentLeft + 1 * margin, getPixRect().getHeight() - 2 * margin);
				contentRect.drawRoundedArea(ctx, margin, 1, new Gray(30), new Gray(30));
			} else {
				Rect r = getPixRect().getReducedBy(margin * 2).copy();
				double spaceBetwheenLabelAndLabel = m_voiceGroupLabel.getPixRect().getLeft()
						- m_presetName.getPixRect().getRight();
				r.set(r.getLeft(), r.getTop(),
						r.getWidth() - (m_voiceGroupLabel.getPixRect().getWidth() + spaceBetwheenLabelAndLabel),
						r.getHeight());
				r.drawRoundedArea(ctx, margin, 1, new Gray(30), new Gray(30));
			}

			if (isDropTarget) {
				getPixRect().getReducedBy(1).drawRoundedArea(ctx, margin, 1, RGBA.transparent(), RGB.red());
			}

			super.draw(ctx, invalidationMask);
		}

		@Override
		public Control click(Position eventPoint) {
			EditBufferUseCases.get().selectVoiceGroup(group);
			return this;
		}

		private class VoiceGroupLabel extends Label {

			VoiceGroupLabel(VoiceGroupSoundSettings parent) {
				super(parent);
			}

			@Override
			protected String crop(Context2d ctx, Rect pixRect, String text) {
				return text;
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
			public RGB getColorFont() {
				if (group == VoiceGroup.I)
					return EditBufferPresenterProvider.getPresenter().voiceGroupI_ForegroundColor;
				return EditBufferPresenterProvider.getPresenter().voiceGroupII_ForegroundColor;
			}
		}

		private class TuneLabel extends Label {

			TuneLabel(VoiceGroupSoundSettings parent) {
				super(parent);
			}

			@Override
			public String getDrawText(Context2d ctx) {
				return "Tune";
			}
		}

		private class VolumeLabel extends Label {

			VolumeLabel(VoiceGroupSoundSettings parent) {
				super(parent);
			}

			@Override
			public String getDrawText(Context2d ctx) {
				return "Volume";
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
			public Control doubleClick(Position p) {
				RenameDialog.open(new Renameable() {

					@Override
					public void setName(String newName) {
						EditBufferUseCases.get().renamePart(newName, group);
					}

					@Override
					public String getTitleName() {
						if (group == VoiceGroup.I)
							return "\uE071";
						else
							return "\uE072";
					}

					@Override
					public String getEntityName() {
						return "Part";
					}

					@Override
					public String getCurrentName() {
						return EditBufferPresenterProvider.getPresenter().currentPartName;
					}
				});
				return this;
			}

			@Override
			public void draw(Context2d ctx, int invalidationMask) {
				getPixRect().drawRoundedArea(ctx, Millimeter.toPixels(0.5), 1, new Gray(68), new Gray(86));
				super.draw(ctx, invalidationMask);
			}
		}

		private class TuneReference extends ValueEdit {
			TuneReference(VoiceGroupSoundSettings parent) {
				super(parent, new ParameterId(360, group), false);
			}
		}

		private class Volume extends ValueEdit {
			Volume(VoiceGroupSoundSettings parent) {
				super(parent, new ParameterId(358, group), false);
			}
		}

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

		private boolean isDropTarget = false;

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
				if (dragProxy.getOrigin() instanceof Preset) {
					Preset p = (Preset) dragProxy.getOrigin();
					if (p.isDual()) {
						choosePresetPart = new ChoosePresetPartDialog(p, group);
					} else {
						NonMaps.get().getServerProxy().loadPresetPartIntoPart(p.getUUID(), VoiceGroup.I, group);
					}
				}

			setIsDropTarget(false);
			return this;
		}
	}

}
