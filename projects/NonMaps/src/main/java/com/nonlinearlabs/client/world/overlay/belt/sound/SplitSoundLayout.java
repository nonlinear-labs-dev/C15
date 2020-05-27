package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.EditBufferPresenter;
import com.nonlinearlabs.client.presenters.EditBufferPresenterProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
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
			addChild(new VoiceGroupSoundSettings(VoiceGroup.I, this));
			addChild(new SplitPoint(this));
			addChild(new VoiceGroupSoundSettings(VoiceGroup.II, this));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double margin = Millimeter.toPixels(2);
			double parts = 20;
			double unit = (w - 2 * margin) / parts;
			double splitPointHeight = Math.min(h, Millimeter.toPixels(30));
			double splitPointWidth = Math.min(3*unit, Millimeter.toPixels(40));
			getChildren().get(0).doLayout(0 * unit, margin, 8 * unit, h - 2 * margin);
			getChildren().get(1).doLayout(9 * unit, (h - splitPointHeight) / 2, splitPointWidth, splitPointHeight);
			getChildren().get(2).doLayout(10 * unit + splitPointWidth, margin, 8 * unit, h - 2 * margin);
		}
	}

	private class SplitPoint extends OverlayLayout {

		public SplitPoint(OverlayLayout parent) {
			super(parent);
			addChild(new SplitPointLabel(this, "Split Point"));
			addChild(new SplitPointValue(this));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			double quarterHeight = h / 4;
			getChildren().get(0).doLayout(0, 0, w, quarterHeight * 2);
			getChildren().get(1).doLayout(0, quarterHeight * 2, w, quarterHeight * 1.1);
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
				super(parent, new ParameterId(356, VoiceGroup.Global));
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


			if(group == VoiceGroup.I) {
				m_voiceGroupLabel.doLayout(margin, middleLine - labelHeight / 2, voiceGroupLabelWidth, labelHeight);
				m_presetName.doLayout(margin + 4 * xunit, margin, presetNameWidth, 5 * yunit);
				m_volumeLabel.doLayout(margin + 4 * xunit, margin + 8 * yunit, parameterLabelWidth, 5 * yunit);
				m_volumeValue.doLayout(margin + 10 * xunit, margin + 8 * yunit, parameterValueWidth, 5 * yunit);
				m_tuneLabel.doLayout(margin + 4 * xunit, margin + 15 * yunit, parameterLabelWidth, 5 * yunit);
				m_tuneValue.doLayout(margin + 10 * xunit, margin + 15 * yunit, parameterValueWidth, 5 * yunit);
			} else {
				m_tuneValue.doLayout(margin, margin + 15 * yunit, parameterValueWidth, 5 * yunit);
				m_tuneLabel.doLayout(margin + parameterValueWidth, margin + 15 * yunit, parameterLabelWidth, 5 * yunit);
				m_volumeValue.doLayout(margin, margin + 8 * yunit, parameterValueWidth, 5 * yunit);
				m_volumeLabel.doLayout(margin + parameterValueWidth, margin + 8 * yunit, parameterLabelWidth, 5 * yunit);
				m_presetName.doLayout(margin, margin, presetNameWidth, 5 * yunit);
				m_voiceGroupLabel.doLayout(w - margin - voiceGroupLabelWidth, middleLine - labelHeight / 2, voiceGroupLabelWidth, labelHeight);
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
			if(group == VoiceGroup.I) {
				Rect contentRect = new Rect(contentLeft - 2 * margin, getPixRect().getTop() + 1 * margin,
						contentRight - contentLeft + 1 * margin, getPixRect().getHeight() - 2 * margin);
				contentRect.drawRoundedArea(ctx, margin, 1, new Gray(30), new Gray(30));
			} else {
				Rect r = getPixRect().getReducedBy(margin*2).copy();
				double spaceBetwheenLabelAndLabel = m_voiceGroupLabel.getPixRect().getLeft() - m_presetName.getPixRect().getRight();
				r.set(r.getLeft(), r.getTop(), r.getWidth() - (m_voiceGroupLabel.getPixRect().getWidth() + spaceBetwheenLabelAndLabel), r.getHeight());
				r.drawRoundedArea(ctx, margin, 1, new Gray(30), new Gray(30));
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
				return EditBufferModel.get().getPresetNameOfVoiceGroup(group);
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
