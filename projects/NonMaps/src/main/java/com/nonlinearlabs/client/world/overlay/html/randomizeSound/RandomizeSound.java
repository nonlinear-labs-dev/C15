package com.nonlinearlabs.client.world.overlay.html.randomizeSound;

import com.google.gwt.core.client.GWT;
import com.google.gwt.dom.client.InputElement;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.SystemSettings;
import com.nonlinearlabs.client.world.overlay.html.Range;

public class RandomizeSound extends Composite {
	interface ThisUiBinder extends UiBinder<HTMLPanel, RandomizeSound> {
	}

	private static ThisUiBinder ourUiBinder = GWT.create(ThisUiBinder.class);

	@UiField
	Label randomizeAmountDisplayString;

	@UiField
	Button apply;

	@UiField
	InputElement randomizeAmount;

	Range randomizeAmountRange;

	public RandomizeSound() {
		initWidget(ourUiBinder.createAndBindUi(this));
		randomizeAmountRange = Range.wrap(randomizeAmount);
		connectEventHandlers();
		connectUpdate();
	}

	public void connectEventHandlers() {
		SystemSettings settings = SystemSettings.get();
		randomizeAmountRange
				.addValueChangeHandler(e -> settings.setRandomizeAmount(randomizeAmountRange.getValue().doubleValue()));

		apply.addClickHandler(e -> {
			EditBufferUseCases.get().randomize();
		});
	}

	public void connectUpdate() {
		DeviceSettingsProvider.get().register(t -> {
			randomizeAmountDisplayString.setText(t.randomizeAmountDisplayString);
			randomizeAmountRange.setValue(t.randomizeAmountValue);
			return true;
		});
	}

	protected boolean applyPresenter(com.nonlinearlabs.client.presenters.PresetSearch t) {
		return isAttached();
	}

}
