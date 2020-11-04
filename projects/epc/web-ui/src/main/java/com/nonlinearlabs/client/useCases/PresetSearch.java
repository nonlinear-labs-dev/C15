package com.nonlinearlabs.client.useCases;

import java.util.Arrays;
import java.util.Comparator;

import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.XMLParser;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.dataModel.presetManager.PresetSearch.SearchQueryCombination;
import com.nonlinearlabs.client.dataModel.presetManager.PresetSearchUpdater;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.presenters.PresetPresenter;
import com.nonlinearlabs.client.presenters.PresetPresenterProviders;

public class PresetSearch {
	private static PresetSearch theSearch = new PresetSearch();

	private com.nonlinearlabs.client.dataModel.presetManager.PresetSearch getModel() {
		return com.nonlinearlabs.client.dataModel.presetManager.PresetSearch.get();
	}

	public static PresetSearch get() {
		return theSearch;
	}

	public void toggleLoadPreset() {
		getModel().loadPreset.setValue(!getModel().loadPreset.getBool());
		refresh();
	}

	public void setCombinationAnd() {
		if (getModel().combination.setValue(SearchQueryCombination.AND))
			refresh();
	}

	public void setCombinationOr() {
		if (getModel().combination.setValue(SearchQueryCombination.OR))
			refresh();
	}

	public void resetQuery() {
		getModel().loadPreset.setValue(false);
		if (getModel().query.setValue(""))
			refresh();
	}

	public void setQuery(String b) {
		getModel().loadPreset.setValue(false);
		if (getModel().query.setValue(b))
			refresh();
	}

	public void toggleSearchInNames() {
		getModel().searchInNames.toggle();
		refresh();
	}

	public void toggleSearchInComments() {
		getModel().searchInComments.toggle();
		refresh();
	}

	public void toggleSearchInDeviceNames() {
		getModel().searchInDeviceNames.toggle();
		refresh();
	}

	public void toggleZoomToAllMatches() {
		getModel().zoomToMatches.toggle();
	}

	public void startSearch() {
		getModel().searchActive.setValue(BooleanValues.on);
	}

	public void finishSearch() {
		getModel().searchActive.setValue(BooleanValues.off);
	}

	public void highlightNext() {
		String[] presets = getSortedMatchingPresets();
		boolean found = false;
		String currentSelection = getModel().currentFilterMatch.getValue();

		for (String p : presets) {
			if (found) {
				getModel().currentFilterMatch.setValue(p);
				return;
			}
			if (p.equals(currentSelection)) {
				found = true;
			}
		}

		if (presets.length > 0)
			getModel().currentFilterMatch.setValue(presets[0]);
	}

	public void highlightPrev() {
		String[] presets = getSortedMatchingPresets();
		String currentSelection = getModel().currentFilterMatch.getValue();
		String old = presets.length > 0 ? presets[presets.length - 1] : "";

		for (String p : presets) {
			if (p.equals(currentSelection)) {
				getModel().currentFilterMatch.setValue(old);
				return;
			}
			old = p;
		}
	}

	public String[] getSortedMatchingPresets() {
		String presets[] = getModel().results.getValue().toArray(new String[0]);
		Arrays.sort(presets, new Comparator<String>() {

			@Override
			public int compare(String uuid1, String uuid2) {
				PresetPresenter p1 = PresetPresenterProviders.get().getPresenter(uuid1);
				PresetPresenter p2 = PresetPresenterProviders.get().getPresenter(uuid1);
				return p1.compare(p2);
			}
		});
		return presets;
	}

	private void refresh() {
		String query = getModel().query.getValue();
		SearchQueryCombination combination = getModel().combination.getValue();
		String fields = "";

		if (getModel().searchInNames.isTrue())
			fields += "name";

		if (getModel().searchInComments.isTrue()) {
			if (!fields.isEmpty())
				fields += ",";
			fields += "comment";
		}

		if (getModel().searchInDeviceNames.isTrue()) {
			if (!fields.isEmpty())
				fields += ",";
			fields += "devicename";
		}

		NonMaps.theMaps.getServerProxy().searchPresets(query, combination, fields, new ServerProxy.DownloadHandler() {

			@Override
			public void onFileDownloaded(String text) {
				if (!query.equals(getModel().query.getValue())) {
					Tracer.log("rejecting outdated preset search response for query " + query);
					return;
				}

				Document xml = XMLParser.parse(text);
				PresetSearchUpdater updater = new PresetSearchUpdater(xml);
				updater.doUpdate();
				sanitizeSelection();
			}

			public void sanitizeSelection() {
				if (!getModel().results.getValue().contains(getModel().currentFilterMatch.getValue())) {
					if (getModel().results.getValue().isEmpty()) {
						getModel().currentFilterMatch.setValue("");
					} else {
						getModel().currentFilterMatch.setValue(getSortedMatchingPresets()[0]);
					}
				}
			}

			@Override
			public void onError() {
				getModel().currentFilterMatch.setValue("");
			}
		});
	}
}
