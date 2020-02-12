package com.nonlinearlabs.client.world.overlay.belt.sound;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.presenters.ParameterPresenterProviders;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.overlay.Label;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class SingleSoundSettings extends OverlayLayout {

	class PresetName extends Label {

		public PresetName(OverlayLayout parent) {
			super(parent);
		}

		@Override
		public String getDrawText(Context2d ctx) {
			return EditBufferModel.get().loadedPresetName.getValue();
		}
	};

	class NamedValueEdit extends OverlayLayout {

		Label label;
		ValueEdit edit;
		ParameterPresenter presenter;

		protected NamedValueEdit(Control parent, ParameterId param) {
			super(parent);
			ParameterPresenterProviders.get().register(param, p -> {
				presenter = p;
				return true;
			});

			label = addChild(new Label(this) {

				@Override
				public String getDrawText(Context2d ctx) {
					return presenter.longName;
				}
			});

			edit = addChild(new ValueEdit(this, param));
		}

		@Override
		public void doLayout(double x, double y, double w, double h) {
			super.doLayout(x, y, w, h);
			edit.doLayout(0, 0, w, h / 2);
			label.doLayout(0, h / 2, w, h / 2);
		}
	}

	PresetName presetName;
	NamedValueEdit tune;
	NamedValueEdit volume;

	protected SingleSoundSettings(Control parent) {
		super(parent);
		presetName = addChild(new PresetName(this));
		tune = addChild(new NamedValueEdit(this, new ParameterId(248, VoiceGroup.Global)));
		volume = addChild(new NamedValueEdit(this, new ParameterId(247, VoiceGroup.Global)));
	}

	public void doLayout(double x, double y, double w, double h) {
		super.doLayout(x, y, w, h);
		int parts = 20;
		double partSize = w / parts;
		presetName.doLayout(0, 0, w, h / 3);
		volume.doLayout(0, h / 3, 9 * partSize, 2 * h / 3);
		tune.doLayout(11 * partSize, h / 3, 9 * partSize, 2 * h / 3);
	}

}
