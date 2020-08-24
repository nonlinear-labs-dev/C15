package com.nonlinearlabs.client.world.overlay.html.presetSearch;

import com.google.gwt.core.client.GWT;
import com.google.gwt.core.client.Scheduler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.CheckBox;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RadioButton;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.Widget;
import com.nonlinearlabs.client.presenters.PresetSearchProvider;

public class PresetSearch extends Composite {
	interface SetupUiBinder extends UiBinder<HTMLPanel, PresetSearch> {
	}

	private static SetupUiBinder ourUiBinder = GWT.create(SetupUiBinder.class);

	@UiField
	RadioButton logicalAnd, logicalOr;

	@UiField
	Button resetQuery, prev, next;

	@UiField
	TextBox search;

	@UiField
	CheckBox searchCriteriaName, searchCriteriaComment, searchCriteriaDeviceName, zoomToAllMatches, loadPreset;

	@UiField
	Label numMatches;

	Widget focussed;

	public PresetSearch() {
		initWidget(ourUiBinder.createAndBindUi(this));

		connectEventHandlers();
		connectUpdate();
	}

	public void connectEventHandlers() {
		search.addKeyUpHandler(b -> {
			if (b.getNativeKeyCode() == KeyCodes.KEY_ESCAPE) {
				getUseCases().finishSearch();
				PresetSearchDialog.hideDialog();
			} else {
				getUseCases().setQuery(search.getText());
			}
		});

		search.addFocusHandler(v -> focussed = search);
		search.addBlurHandler(v -> focussed = null);

		logicalAnd.addValueChangeHandler(b -> getUseCases().setCombinationAnd());
		logicalOr.addValueChangeHandler(b -> getUseCases().setCombinationOr());
		resetQuery.addClickHandler(b -> getUseCases().resetQuery());

		searchCriteriaName.addClickHandler(b -> getUseCases().toggleSearchInNames());
		searchCriteriaComment.addClickHandler(b -> getUseCases().toggleSearchInComments());
		searchCriteriaDeviceName.addClickHandler(b -> getUseCases().toggleSearchInDeviceNames());

		zoomToAllMatches.addClickHandler(b -> getUseCases().toggleZoomToAllMatches());

		loadPreset.addClickHandler(b -> getUseCases().toggleLoadPreset());

		prev.addClickHandler(b -> getUseCases().highlightPrev());
		next.addClickHandler(b -> getUseCases().highlightNext());

		Scheduler.get().scheduleDeferred((Command)() -> {
			search.selectAll();
			search.setFocus(true);
		});
	}

	public com.nonlinearlabs.client.useCases.PresetSearch getUseCases() {
		return com.nonlinearlabs.client.useCases.PresetSearch.get();
	}

	public void connectUpdate() {
		PresetSearchProvider.get().register(p -> applyPresenter(p));
	}

	protected boolean applyPresenter(com.nonlinearlabs.client.presenters.PresetSearch t) {
		if (focussed != search)
			search.setText(t.query);

		logicalAnd.setValue(t.andCombination);
		logicalOr.setValue(t.orCombination);
		searchCriteriaName.setValue(t.searchInNames);
		searchCriteriaComment.setValue(t.searchInComments);
		searchCriteriaDeviceName.setValue(t.searchInDeviceNames);
		zoomToAllMatches.setValue(t.zoomToAllMatches);
		numMatches.setText(t.numMatches);
		loadPreset.setValue(t.loadPreset);
		return isAttached();
	}

}
