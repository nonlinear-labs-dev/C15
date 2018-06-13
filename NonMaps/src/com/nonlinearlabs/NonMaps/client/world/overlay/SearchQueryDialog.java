package com.nonlinearlabs.NonMaps.client.world.overlay;

import java.util.List;

import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.RepeatingCommand;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.event.dom.client.MouseDownEvent;
import com.google.gwt.event.dom.client.MouseDownHandler;
import com.google.gwt.event.dom.client.MouseMoveEvent;
import com.google.gwt.event.dom.client.MouseUpEvent;
import com.google.gwt.event.logical.shared.ValueChangeEvent;
import com.google.gwt.event.logical.shared.ValueChangeHandler;
import com.google.gwt.user.client.Command;
import com.google.gwt.user.client.Event;
import com.google.gwt.user.client.Event.NativePreviewEvent;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.CheckBox;
import com.google.gwt.user.client.ui.FlowPanel;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.Panel;
import com.google.gwt.user.client.ui.RadioButton;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.world.Rect;
import com.nonlinearlabs.NonMaps.client.world.maps.NonRect;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.NonMaps.client.world.maps.presets.PresetManager.SearchQueryFields;

public class SearchQueryDialog extends GWTDialog {

	private class NameFieldMouseUpHandler implements MouseDownHandler {
		TextBox nameField;

		public NameFieldMouseUpHandler(TextBox nameField) {
			this.nameField = nameField;
		}

		@Override
		public void onMouseDown(MouseDownEvent event) {
			nameField.setFocus(true);
		}
	}

	private class TriggerNewSearchHandlerOnClick implements ValueChangeHandler<Boolean> {
		CheckBox b;
		SearchFieldManager sm;

		public TriggerNewSearchHandlerOnClick(CheckBox b, SearchFieldManager sm) {
			this.b = b;
			this.sm = sm;
		}

		@Override
		public void onValueChange(ValueChangeEvent<Boolean> event) {
			sm.onCheckChanged(b);
		}
	}

	private class SearchFieldManager {
		CheckBox name;
		CheckBox comment;
		CheckBox deviceName;

		public SearchFieldManager(CheckBox n, CheckBox c, CheckBox d) {
			name = n;
			comment = c;
			deviceName = d;
		}

		private PresetManager getPresetManager() {
			return NonMaps.get().getNonLinearWorld().getPresetManager();
		}

		private void addIfNotPresent(List<SearchQueryFields> currentFields, SearchQueryFields o) {
			if (currentFields.contains(o) == false)
				currentFields.add(o);
		}

		private void removeIfPresent(List<SearchQueryFields> currentFields, SearchQueryFields o) {
			if (currentFields.contains(o))
				currentFields.remove(o);
		}

		private void allowFieldInList(List<SearchQueryFields> currentFields, SearchQueryFields o, boolean allow) {
			if (allow) {
				addIfNotPresent(currentFields, o);
			} else {
				removeIfPresent(currentFields, o);
			}
		}

		public boolean isFieldAllowed(SearchQueryFields field) {
			return getPresetManager().getFieldsToBeSearched().contains(field);
		}

		public void onCheckChanged(CheckBox b) {
			List<PresetManager.SearchQueryFields> currentFields = getPresetManager().getFieldsToBeSearched();
			if (b == name) {
				allowFieldInList(currentFields, SearchQueryFields.name, b.getValue());
			} else if (b == comment) {
				allowFieldInList(currentFields, SearchQueryFields.comment, b.getValue());
			} else if (b == deviceName) {
				allowFieldInList(currentFields, SearchQueryFields.devicename, b.getValue());
			}
			getPresetManager().setFieldsToBeSearched(currentFields);
		}
	}

	private class NameFieldKeyUpHandler implements KeyUpHandler {
		TextBox nameField;

		public NameFieldKeyUpHandler(TextBox nameField) {
			this.nameField = nameField;
		}

		@Override
		public void onKeyUp(KeyUpEvent event) {
			timer = new RepeatingCommand() {

				@Override
				public boolean execute() {
					if (timer == this) {
						String query = nameField.getText();
						NonMaps.theMaps.getNonLinearWorld().getPresetManager().setFilter(query);

						if (query.isEmpty())
							restoreZoom();
						else
							storeZoom();
					}
					return false;
				}
			};

			Scheduler.get().scheduleFixedDelay(timer, 250);
		}
	}

	private SearchFieldManager theFieldManager;
	private static SearchQueryDialog theDialog;
	private RepeatingCommand timer;
	private Label matches;
	private NonRect storedZoom;

	private SearchQueryDialog() {
		RootPanel.get().add(this);

		initalShow();

		addHeader("Search presets for ...");
		setAnimationEnabled(true);
		setGlassEnabled(false);
		setModal(false);

		VerticalPanel rootPanel = new VerticalPanel();
		rootPanel.add(createTextInputPanel());
		rootPanel.add(createAndOrPanel());
		rootPanel.add(createSpacer());
		rootPanel.add(createCheckBoxPanel());
		rootPanel.add(createSpacer());
		rootPanel.add(createZoomPanel());
		rootPanel.add(createSpacer());
		rootPanel.add(createButtonPanel());
		setWidget(rootPanel);
	}
		
	private FlowPanel createSpacer() {
		FlowPanel dividerPanel = new FlowPanel();
		HTML html = new HTML("<hr  style=\"width:98%;border: 0;height: 1px;background: #a5a5a5;background-image: linear-gradient(to right, #a5a5a5,#a5a5a5,#a5a5a5);\" />");
		dividerPanel.add(html);
		return dividerPanel;
	}

	private FlowPanel createCheckBoxPanel() {
		FlowPanel buttonPanel = new FlowPanel();
		CheckBox name = new CheckBox("Name");
		CheckBox comment = new CheckBox("Comment");
		CheckBox deviceName = new CheckBox("Device Name");

		theFieldManager = new SearchFieldManager(name, comment, deviceName);

		name.setValue(theFieldManager.isFieldAllowed(SearchQueryFields.name));
		comment.setValue(theFieldManager.isFieldAllowed(SearchQueryFields.comment));
		deviceName.setValue(theFieldManager.isFieldAllowed(SearchQueryFields.devicename));

		name.addValueChangeHandler((ValueChangeHandler<Boolean>) new TriggerNewSearchHandlerOnClick(name, theFieldManager));
		comment.addValueChangeHandler((ValueChangeHandler<Boolean>) new TriggerNewSearchHandlerOnClick(comment, theFieldManager));
		deviceName.addValueChangeHandler((ValueChangeHandler<Boolean>) new TriggerNewSearchHandlerOnClick(deviceName, theFieldManager));

		name.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.theMaps.getNonLinearWorld().getPresetManager()
						.setFilter(NonMaps.theMaps.getNonLinearWorld().getPresetManager().getFilter());
			}
		});

		comment.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.theMaps.getNonLinearWorld().getPresetManager()
						.setFilter(NonMaps.theMaps.getNonLinearWorld().getPresetManager().getFilter());
			}
		});

		deviceName.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				NonMaps.theMaps.getNonLinearWorld().getPresetManager()
						.setFilter(NonMaps.theMaps.getNonLinearWorld().getPresetManager().getFilter());
			}
		});

		buttonPanel.add(name);
		buttonPanel.add(comment);
		buttonPanel.add(deviceName);

		return buttonPanel;
	}

	private FlowPanel createZoomPanel() {
		FlowPanel zoomPanel = new FlowPanel();
		CheckBox zoomToAllMatches = new CheckBox("Zoom to all matches");
		zoomToAllMatches.getElement().addClassName("auto-zoom");
		Boolean b = Boolean.valueOf(NonMaps.theMaps.getNonLinearWorld().getSettings().get("search-auto-zoom", "false"));
		zoomToAllMatches.setValue(b);
		
		zoomToAllMatches.addValueChangeHandler(new ValueChangeHandler<Boolean>() {

			@Override
			public void onValueChange(ValueChangeEvent<Boolean> event) {
				NonMaps.theMaps.getNonLinearWorld().getSettings().set("search-auto-zoom", event.getValue().toString());

				if (event.getValue())
					zoomToAll();
			}
		});
		
		zoomPanel.add(zoomToAllMatches);
		return zoomPanel;
	}
	
	private FlowPanel createButtonPanel() {
		FlowPanel buttonPanel = new FlowPanel();
		Button nextMatch = new Button("Next");
		Button previousMatch = new Button("Previous");
		previousMatch.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				highlightPreviousMatch();
			}
		});

		nextMatch.addClickHandler(new ClickHandler() {

			@Override
			public void onClick(ClickEvent event) {
				highlightNextMatch();
			}
		});
		
		previousMatch.getElement().addClassName("previous-button");
		nextMatch.getElement().addClassName("next-button");
		
		buttonPanel.add(previousMatch);
		buttonPanel.add(nextMatch);
		return buttonPanel;
	}

	private Panel createTextInputPanel() {
		HorizontalPanel panel = new HorizontalPanel();

		final TextBox nameField = new TextBox();
		final Label l = new Label();
		l.setText("⌫");
		l.setHeight("2em");
		nameField.getElement().addClassName("search-query");
		nameField.addKeyUpHandler(new NameFieldKeyUpHandler(nameField));
		nameField.addMouseDownHandler(new NameFieldMouseUpHandler(nameField));

		nameField.addMouseDownHandler(new MouseDownHandler() {

			@Override
			public void onMouseDown(MouseDownEvent event) {
				double x = event.getRelativeX(nameField.getElement());
				double w = nameField.getElement().getClientWidth();
				double activeAreaWidth = Millimeter.toPixels(10) / (NonMaps.getPixelFactor() * NonMaps.devicePixelRatio);

				if (x > (w - activeAreaWidth)) {
					nameField.setText("");
					NonMaps.theMaps.getNonLinearWorld().getPresetManager().setFilter("");
					restoreZoom();
				}
			}
		});

		Scheduler.get().scheduleFinally(new Command() {
			public void execute() {
				nameField.setFocus(true);
			}
		});

		l.addStyleName("search-query-symbol");
		
		panel.add(l);
		panel.add(nameField);
		panel.add(matches = new Label(""));
		updateMatches(0);
		return panel;
	}

	private FlowPanel createAndOrPanel() {
		FlowPanel panel = new FlowPanel();
		RadioButton and = new RadioButton("AndOr", "AND");
		RadioButton or = new RadioButton("AndOr", "OR");

		and.addValueChangeHandler(new ValueChangeHandler<Boolean>() {

			@Override
			public void onValueChange(ValueChangeEvent<Boolean> event) {
				NonMaps.theMaps.getNonLinearWorld().getPresetManager().setSearchQueryCombination(PresetManager.SearchQueryCombination.AND);
			}
		});

		or.addValueChangeHandler(new ValueChangeHandler<Boolean>() {

			@Override
			public void onValueChange(ValueChangeEvent<Boolean> event) {
				NonMaps.theMaps.getNonLinearWorld().getPresetManager().setSearchQueryCombination(PresetManager.SearchQueryCombination.OR);
			}
		});

		and.setValue(
				NonMaps.theMaps.getNonLinearWorld().getPresetManager().getSearchQueryCombination() == PresetManager.SearchQueryCombination.AND,
				false);
		or.setValue(
				NonMaps.theMaps.getNonLinearWorld().getPresetManager().getSearchQueryCombination() == PresetManager.SearchQueryCombination.OR,
				false);

		panel.add(and);
		panel.add(or);
		return panel;
	}

	protected void highlightNextMatch() {
		NonMaps.theMaps.getNonLinearWorld().getPresetManager().highlightNextFilterMatch();
	}

	protected void highlightPreviousMatch() {
		NonMaps.theMaps.getNonLinearWorld().getPresetManager().highlightPreviousFilterMatch();
	}

	protected void zoomToAll() {
		NonMaps.theMaps.getNonLinearWorld().getPresetManager().zoomToAllFilterMatches();
	}

	@Override
	protected void onPreviewNativeEvent(NativePreviewEvent event) {
		super.onPreviewNativeEvent(event);
		switch (event.getTypeInt()) {
		case Event.ONKEYDOWN:
			if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ESCAPE) {
				commit();
			} else if (event.getNativeEvent().getKeyCode() == KeyCodes.KEY_ENTER) {
				commit();
			}
			break;
		}
	}

	@Override
	protected void commit() {
		theDialog.hide();
		NonMaps.theMaps.getNonLinearWorld().getPresetManager().clearFilter();
		NonMaps.theMaps.captureFocus();
		theDialog = null;
		restoreZoom();
		NonMaps.theMaps.getNonLinearWorld().requestLayout();
	}

	public static void toggle() {
		if (theDialog != null) {
			theDialog.commit();
		} else {
			if (!NonMaps.theMaps.getNonLinearWorld().getPresetManager().isEmpty()) {
				theDialog = new SearchQueryDialog();
				theDialog.pushDialogToFront();
			}
		}
	}

	public static boolean isShown() {
		return theDialog != null;
	}

	public void updateMatches(int i) {
		if (i == 0)
			matches.setText("No matches.");
		else if (i == 1)
			matches.setText("One match.");
		else
			matches.setText(i + " matches.");
	}

	public static void setMatches(int i) {
		if (theDialog != null)
			theDialog.updateMatches(i);
	}

	private void storeZoom() {
		if (storedZoom == null)
			storedZoom = NonMaps.theMaps.getNonLinearWorld().getViewport().getNonPosition().copy();
	}

	private void restoreZoom() {
		if (storedZoom != null) {
			NonMaps.theMaps.getNonLinearWorld().animateViewport(storedZoom, true);
			storedZoom = null;
		}
	}

	static int lastPopupLeft = -1;
	static int lastPopupTop = -1;

	@Override
	protected void setLastPopupPos(int popupLeft, int popupTop) {
		lastPopupLeft = popupLeft;
		lastPopupTop = popupTop;
	}

	@Override
	protected int getLastPopupPosTop() {
		return lastPopupTop;
	}

	@Override
	protected int getLastPopupPosLeft() {
		return lastPopupLeft;
	}
}