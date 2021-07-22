package com.nonlinearlabs.client.world.overlay.html.setup;

import java.util.Map;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;

import com.google.gwt.core.client.GWT;
import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.dom.client.DivElement;
import com.google.gwt.dom.client.InputElement;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.dom.client.Style.Display;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ChangeHandler;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.CheckBox;
import com.google.gwt.user.client.ui.Composite;
import com.google.gwt.user.client.ui.FileUpload;
import com.google.gwt.user.client.ui.HTMLPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.RadioButton;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.xhr.client.XMLHttpRequest;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.presetManager.Bank;
import com.nonlinearlabs.client.dataModel.presetManager.PresetManagerModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.AftertouchCCMapping;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.AftertouchCurve;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BenderCCMapping;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BenderCurve;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.DisplayScaling;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.EditParameter;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.MidiReceiveChannel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.MidiReceiveChannelSplit;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.MidiSendChannel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.MidiSendChannelSplit;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.PedalCCMapping;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.PedalType;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.RibbonCCMapping;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.SelectionAutoScroll;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.StripeBrightness;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.VelocityCurve;
import com.nonlinearlabs.client.presenters.DeviceInformation;
import com.nonlinearlabs.client.presenters.DeviceInformationProvider;
import com.nonlinearlabs.client.presenters.DeviceSettings;
import com.nonlinearlabs.client.presenters.DeviceSettingsProvider;
import com.nonlinearlabs.client.presenters.LocalSettings;
import com.nonlinearlabs.client.presenters.LocalSettingsProvider;
import com.nonlinearlabs.client.presenters.MidiSettings;
import com.nonlinearlabs.client.presenters.MidiSettingsProvider;
import com.nonlinearlabs.client.tools.Pair;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.SystemSettings;
import com.nonlinearlabs.client.world.overlay.html.Range;
import com.nonlinearlabs.client.presenters.MidiSettings.RoutingEntry;

public class Setup extends Composite {
	interface SetupUiBinder extends UiBinder<HTMLPanel, Setup> {
	}

	private static SetupUiBinder ourUiBinder = GWT.create(SetupUiBinder.class);

	@UiField
	Button deviceSettingsButton, uiSettingsButton, uiMidiButton, uiFlacButton, systemInfoButton, aboutButton;

	@UiField
	FileUpload updateFile;

	@UiField
	DivElement deviceSettings, uiSettings, midiSettings, flacSettings, systemInfo, about, updateSpinner;

	@UiField
	ListBox velocityCurve, aftertouchCurve, benderCurve, pedal1Type, pedal2Type, pedal3Type, pedal4Type,
			selectionAutoScroll, editParameter, scalingFactor, stripeBrightness, midiReceiveChannel, midiReceiveChannelSplit,
			midiSendChannel, midiSendChannelSplit, pedal1Mapping, 
			pedal2Mapping, pedal3Mapping, pedal4Mapping, ribbon1Mapping, ribbon2Mapping, benderMapping, aftertouchMapping, pcBanks;

	@UiField
	Label pedal1DisplayString, pedal2DisplayString, pedal3DisplayString, pedal4DisplayString,
			editSmoothingTimeDisplayString, freeMemory, uiVersion, dateTime, uiHead, uiBranch, uiCommits, uiCommitDate, uiUsedRam, uiTotalRam;

	@UiField
	InputElement pedal1Slider, pedal2Slider, pedal3Slider, pedal4Slider;

	@UiField
	RadioButton presetGlitchSuppressionOn, presetGlitchSuppressionOff, showContextMenusOn, showContextMenusOff,
			presetDragDropOn, presetDragDropOff, bitmapCacheOn, bitmapCacheOff, developerOptionsOn, developerOptionsOff,
			highlightChangedOn, highlightChangedOff, syncPartsOn, syncPartsOff, globalLocalOn, 
			globalLocalOff, highVeloCCOn, highVeloCCOff, enable14Bit, disable14Bit, autoStartRecordOn, autoStartRecordOff;

	@UiField
	Label transitionTimeDisplayString, tuneReferenceDisplayString;

	@UiField
	InputElement transitionTimeSlider, tuneReferenceSlider;

	Range transitionTimeSliderRange, tuneReferenceSliderRange;

	@UiField
	InputElement editSmoothingTimeSlider;

	@UiField
	TextArea deviceName;

	@UiField
	Button saveDeviceName, storeInitSound, resetInitSound, classicMidi, highResMidi, panicAE, routingsOn, routingsOff;

	Range editSmoothingTimeRange;
	Range pedal1Range, pedal2Range, pedal3Range, pedal4Range;

	@UiField
	CheckBox psonNotes, psonPC, pson0, pson1, pson2, pson3, pson4, pson5, pson6, pson7, pronNotes, pronPC, pron0, pron1, pron2, pron3, pron4, pron5, pron6, pron7, 
				ssonNotes, ssonPC, sson0, sson1, sson2, sson3, sson4, sson5, sson6, sson7, sronNotes, sronPC, sron0, sron1, sron2, sron3, sron4, sron5, sron6, sron7, 
				lonNotes, lon0, lon1, lon2, lon3, lon4, lon5, lon6, lon7;


	private class RoutingsMap {
		public RoutingsMap(CheckBox pson, CheckBox pron, CheckBox sson, CheckBox sron, CheckBox lon) {
			primSendOn = pson;
			primRecOn = pron;
			splitSendOn = sson;
			splitRecOn = sron;
			localOn = lon;
		}

		private CheckBox primSendOn, primRecOn, splitSendOn, splitRecOn, localOn;
		
		//this order corresponds with the enum found in the MidiSettingsMessage here: nltools/messaging/Message.h 
		public CheckBox getCheckBoxForOptionIndex(int i) {
			switch(i) {
				case 0:
					return primSendOn;
				case 1:
					return primRecOn;
				case 2:
					return splitSendOn;
				case 3:
					return splitRecOn;
				case 4:
					return localOn;
			}
			return null;
		}

	} 

	private RoutingsMap[] m_routingMap;

	private void createUIData() {
		m_routingMap = new RoutingsMap[10];
		m_routingMap[0] = new RoutingsMap(pson0, pron0, sson0, sron0, lon0);
		m_routingMap[1] = new RoutingsMap(pson1, pron1, sson1, sron1, lon1);
		m_routingMap[2] = new RoutingsMap(pson2, pron2, sson2, sron2, lon2);
		m_routingMap[3] = new RoutingsMap(pson3, pron3, sson3, sron3, lon3);
		m_routingMap[4] = new RoutingsMap(pson4, pron4, sson4, sron4, lon4);
		m_routingMap[5] = new RoutingsMap(pson5, pron5, sson5, sron5, lon5);
		m_routingMap[6] = new RoutingsMap(pson6, pron6, sson6, sron6, lon6);
		m_routingMap[7] = new RoutingsMap(pson7, pron7, sson7, sron7, lon7);
		m_routingMap[8] = new RoutingsMap(psonNotes, pronNotes, ssonNotes, sronNotes, lonNotes);
		m_routingMap[9] = new RoutingsMap(psonPC, pronPC, ssonPC, sronPC, null);
	}

	public Setup() {
		initWidget(ourUiBinder.createAndBindUi(this));

		transitionTimeSliderRange = Range.wrap(transitionTimeSlider);
		tuneReferenceSliderRange = Range.wrap(tuneReferenceSlider);

		pedal1Range = Range.wrap(pedal1Slider);
		pedal2Range = Range.wrap(pedal2Slider);
		pedal3Range = Range.wrap(pedal3Slider);
		pedal4Range = Range.wrap(pedal4Slider);

		createUIData();

		setupTexts();
		connectEventHandlers();
		connectUpdate();

		switchPage(deviceSettingsButton, deviceSettings);
	}

	public void setupTexts() {
		updateSpinner.addClassName("hidden");

		NonMaps.get().getServerProxy().downloadEnumStrings("PedalTypes", (options) -> {
			fillListboxWithOptions(pedal1Type, options);
			fillListboxWithOptions(pedal2Type, options);
			fillListboxWithOptions(pedal3Type, options);
			fillListboxWithOptions(pedal4Type, options);

			DeviceSettings t = DeviceSettingsProvider.get().getPresenter();

			applyPedalValues(t.pedal1, pedal1Type, pedal1Range, pedal1DisplayString);
			applyPedalValues(t.pedal2, pedal2Type, pedal2Range, pedal2DisplayString);
			applyPedalValues(t.pedal3, pedal3Type, pedal3Range, pedal3DisplayString);
			applyPedalValues(t.pedal4, pedal4Type, pedal4Range, pedal4DisplayString);
		});

		fillListboxWithOptions(midiReceiveChannel, MidiSettings.ReceiveChannel.options);
		fillListboxWithOptions(midiReceiveChannelSplit, MidiSettings.ReceiveChannelSplit.options);

		fillListboxWithOptions(midiSendChannel, MidiSettings.SendChannel.options);
		fillListboxWithOptions(midiSendChannelSplit, MidiSettings.SendChannelSplit.options);

		fillListboxWithOptions(velocityCurve, DeviceSettings.VelocityCurve.options);
		fillListboxWithOptions(aftertouchCurve, DeviceSettings.AftertouchCurve.options);
		fillListboxWithOptions(benderCurve, DeviceSettings.BenderCurve.options);
		fillListboxWithOptions(selectionAutoScroll, LocalSettings.SelectionAutoScroll.options);
		fillListboxWithOptions(editParameter, LocalSettings.EditParameter.options);
		fillListboxWithOptions(scalingFactor, LocalSettings.DisplayScalingFactor.options);
		fillListboxWithOptions(stripeBrightness, LocalSettings.StripeBrightness.options);
		fillListboxWithOptions(pedal1Mapping, MidiSettings.PedalMapping.options);
		fillListboxWithOptions(pedal2Mapping, MidiSettings.PedalMapping.options);
		fillListboxWithOptions(pedal3Mapping, MidiSettings.PedalMapping.options);
		fillListboxWithOptions(pedal4Mapping, MidiSettings.PedalMapping.options);
		fillListboxWithOptions(ribbon1Mapping, MidiSettings.RibbonMapping.options);
		fillListboxWithOptions(ribbon2Mapping, MidiSettings.RibbonMapping.options);
		fillListboxWithOptions(benderMapping, MidiSettings.BenderMapping.options);
		fillListboxWithOptions(aftertouchMapping, MidiSettings.AftertouchMapping.options);

		fillRadioButtons(presetGlitchSuppressionOn, presetGlitchSuppressionOff,
				DeviceSettings.PresetGlitchSuppression.options);
		fillRadioButtons(showContextMenusOn, showContextMenusOff, LocalSettings.ContextMenus.options);
		fillRadioButtons(presetDragDropOn, presetDragDropOff, LocalSettings.PresetDragDrop.options);
		fillRadioButtons(bitmapCacheOn, bitmapCacheOff, LocalSettings.BitmapCache.options);
		fillRadioButtons(developerOptionsOn, developerOptionsOff, LocalSettings.ShowDeveloperOptions.options);
		fillRadioButtons(highlightChangedOn, highlightChangedOff, DeviceSettings.HighlightChanged.options);
		fillRadioButtons(syncPartsOn, syncPartsOff, DeviceSettings.SyncPartsAcrossUI.options);
		fillRadioButtons(highVeloCCOn, highVeloCCOff, MidiSettings.OnOffOption.options);
		fillRadioButtons(enable14Bit, disable14Bit, MidiSettings.OnOffOption.options);
		fillRadioButtons(autoStartRecordOn, autoStartRecordOff, MidiSettings.OnOffOption.options);
		fillRadioButtons(globalLocalOn, globalLocalOff, MidiSettings.OnOffOption.options);
	}

	public void connectEventHandlers() {
		SystemSettings settings = SystemSettings.get();
		com.nonlinearlabs.client.useCases.LocalSettings locals = com.nonlinearlabs.client.useCases.LocalSettings.get();

		deviceSettingsButton.addClickHandler(e -> switchPage(deviceSettingsButton, deviceSettings));
		uiSettingsButton.addClickHandler(e -> switchPage(uiSettingsButton, uiSettings));
		systemInfoButton.addClickHandler(e -> switchPage(systemInfoButton, systemInfo));
		aboutButton.addClickHandler(e -> switchPage(aboutButton, about));
		uiMidiButton.addClickHandler(e -> switchPage(uiMidiButton, midiSettings));
		uiFlacButton.addClickHandler(e -> switchPage(uiFlacButton, flacSettings));

		velocityCurve.addChangeHandler(
				e -> settings.setVelocityCurve(VelocityCurve.values()[velocityCurve.getSelectedIndex()]));
		aftertouchCurve.addChangeHandler(
				e -> settings.setAftertouchCurve(AftertouchCurve.values()[aftertouchCurve.getSelectedIndex()]));
		benderCurve
				.addChangeHandler(e -> settings.setBenderCurve(BenderCurve.values()[benderCurve.getSelectedIndex()]));
		pedal1Type.addChangeHandler(e -> settings.setPedal1Type(PedalType.values()[pedal1Type.getSelectedIndex()]));
		pedal2Type.addChangeHandler(e -> settings.setPedal2Type(PedalType.values()[pedal2Type.getSelectedIndex()]));
		pedal3Type.addChangeHandler(e -> settings.setPedal3Type(PedalType.values()[pedal3Type.getSelectedIndex()]));
		pedal4Type.addChangeHandler(e -> settings.setPedal4Type(PedalType.values()[pedal4Type.getSelectedIndex()]));
		selectionAutoScroll.addChangeHandler(e -> locals
				.setSelectionAutoScroll(SelectionAutoScroll.values()[selectionAutoScroll.getSelectedIndex()]));
		editParameter.addChangeHandler(
				e -> locals.setEditParameter(EditParameter.values()[editParameter.getSelectedIndex()]));
		scalingFactor.addChangeHandler(
				e -> locals.setDisplayScaling(DisplayScaling.values()[scalingFactor.getSelectedIndex()]));
		stripeBrightness.addChangeHandler(
				e -> locals.setStripeBrightness(StripeBrightness.values()[stripeBrightness.getSelectedIndex()]));

		presetGlitchSuppressionOn.addClickHandler(e -> settings.setPresetGlitchSuppression(BooleanValues.on));
		presetGlitchSuppressionOff.addClickHandler(e -> settings.setPresetGlitchSuppression(BooleanValues.off));

		showContextMenusOn.addClickHandler(e -> locals.setContextMenus(BooleanValues.on));
		showContextMenusOff.addClickHandler(e -> locals.setContextMenus(BooleanValues.off));

		presetDragDropOn.addClickHandler(e -> locals.setPresetDragDrop(BooleanValues.on));
		presetDragDropOff.addClickHandler(e -> locals.setPresetDragDrop(BooleanValues.off));

		bitmapCacheOn.addClickHandler(e -> locals.setBitmapCache(BooleanValues.on));
		bitmapCacheOff.addClickHandler(e -> locals.setBitmapCache(BooleanValues.off));

		developerOptionsOn.addClickHandler(e -> locals.showDeveloperOptions(BooleanValues.on));
		developerOptionsOff.addClickHandler(e -> locals.showDeveloperOptions(BooleanValues.off));

		editSmoothingTimeRange = Range.wrap(editSmoothingTimeSlider);
		editSmoothingTimeRange.addValueChangeHandler(
				e -> settings.setEditSmoothingTime(editSmoothingTimeRange.getValue().doubleValue()));

		syncPartsOn.addClickHandler(e -> settings.setSyncParts(BooleanValues.on));
		syncPartsOff.addClickHandler(e -> settings.setSyncParts(BooleanValues.off));

		pedal1Range.addValueChangeHandler(e -> EditBufferUseCases.get()
				.setParameterValue(new ParameterId(254, VoiceGroup.Global), e.getValue().doubleValue(), true));
		pedal2Range.addValueChangeHandler(e -> EditBufferUseCases.get()
				.setParameterValue(new ParameterId(259, VoiceGroup.Global), e.getValue().doubleValue(), true));
		pedal3Range.addValueChangeHandler(e -> EditBufferUseCases.get()
				.setParameterValue(new ParameterId(264, VoiceGroup.Global), e.getValue().doubleValue(), true));
		pedal4Range.addValueChangeHandler(e -> EditBufferUseCases.get()
				.setParameterValue(new ParameterId(269, VoiceGroup.Global), e.getValue().doubleValue(), true));

		saveDeviceName.addClickHandler(e -> settings.setDeviceName(deviceName.getValue()));

		highlightChangedOn.addValueChangeHandler(e -> settings.setHighlightChangedParameters(BooleanValues.on));
		highlightChangedOff.addValueChangeHandler(e -> settings.setHighlightChangedParameters(BooleanValues.off));

		transitionTimeSliderRange.addValueChangeHandler(v -> settings.setTransitionTime(v.getValue().doubleValue()));
		tuneReferenceSliderRange.addValueChangeHandler(v -> settings.setTuneReference(v.getValue().doubleValue()));

		resetInitSound.addClickHandler(e -> settings.resetInitSound());
		storeInitSound.addClickHandler(e -> settings.storeInitSound());

		midiReceiveChannel.addChangeHandler(e -> settings.setReceiveMidiChannel(MidiReceiveChannel.values()[midiReceiveChannel.getSelectedIndex()]));
		midiReceiveChannelSplit.addChangeHandler(e -> settings.setReceiveMidiChannelSplit(MidiReceiveChannelSplit.values()[midiReceiveChannelSplit.getSelectedIndex()]));

		midiSendChannel.addChangeHandler(e -> settings.setSendChannel(MidiSendChannel.values()[midiSendChannel.getSelectedIndex()]));
		midiSendChannelSplit.addChangeHandler(e -> settings.setSendChannelSplit(MidiSendChannelSplit.values()[midiSendChannelSplit.getSelectedIndex()]));

		pedal1Mapping.addChangeHandler(e -> settings.setPedal1Mapping(PedalCCMapping.values()[pedal1Mapping.getSelectedIndex()]));
		pedal2Mapping.addChangeHandler(e -> settings.setPedal2Mapping(PedalCCMapping.values()[pedal2Mapping.getSelectedIndex()]));
		pedal3Mapping.addChangeHandler(e -> settings.setPedal3Mapping(PedalCCMapping.values()[pedal3Mapping.getSelectedIndex()]));
		pedal4Mapping.addChangeHandler(e -> settings.setPedal4Mapping(PedalCCMapping.values()[pedal4Mapping.getSelectedIndex()]));
		ribbon1Mapping.addChangeHandler(e -> settings.setRibbon1Mapping(RibbonCCMapping.values()[ribbon1Mapping.getSelectedIndex()]));
		ribbon2Mapping.addChangeHandler(e -> settings.setRibbon2Mapping(RibbonCCMapping.values()[ribbon2Mapping.getSelectedIndex()]));
		aftertouchMapping.addChangeHandler(e -> settings.setAftertouchMapping(AftertouchCCMapping.values()[aftertouchMapping.getSelectedIndex()]));
		benderMapping.addChangeHandler(e -> settings.setPitchbendMapping(BenderCCMapping.values()[benderMapping.getSelectedIndex()]));
		highVeloCCOn.addValueChangeHandler(e -> settings.setHighVelocityCC(BooleanValues.on));
		highVeloCCOff.addValueChangeHandler(e -> settings.setHighVelocityCC(BooleanValues.off));

		pcBanks.addChangeHandler(e -> {
			NonMaps.get().getServerProxy().selectMidiBank(pcBanks.getSelectedValue());
		});

		autoStartRecordOn.addValueChangeHandler(e -> settings.setAutoStartRecorder(BooleanValues.on));
		autoStartRecordOff.addValueChangeHandler(e -> settings.setAutoStartRecorder(BooleanValues.off));

		enable14Bit.addValueChangeHandler(e -> settings.set14BitSupport(BooleanValues.on));
		disable14Bit.addValueChangeHandler(e -> settings.set14BitSupport(BooleanValues.off));	
		
		panicAE.addClickHandler(e -> settings.panic());

		classicMidi.addClickHandler(e -> settings.resetToClassicMidi());
		highResMidi.addClickHandler(e -> settings.resetToHighResMidi());
		routingsOn.addClickHandler(e -> settings.resetRoutings(true));
		routingsOff.addClickHandler(e -> settings.resetRoutings(false));

		globalLocalOn.addValueChangeHandler(e -> settings.setGlobalLocal(BooleanValues.on));
		globalLocalOff.addValueChangeHandler(e -> settings.setGlobalLocal(BooleanValues.off));

		updateFile.addChangeHandler(new ChangeHandler() {

			@Override
			public void onChange(ChangeEvent e) {
				updateSpinner.removeClassName("hidden");
				updateFile.getElement().addClassName("hidden");

				loadUpdateFile(e.getNativeEvent(), new TarUploadedHandler(){
					@Override
					public void onTarUploaded(JavaScriptObject buffer) {
						NonMaps.theMaps.getServerProxy().uploadUpdate(buffer, new UploadDoneReceiver(){

							@Override
							public void onUploadFinished(XMLHttpRequest answer) {
								updateSpinner.addClassName("hidden");
								updateFile.getElement().removeClassName("hidden");
							}
						});
					}
				});
			}
		});


		int entryIndex = 0;
		for(RoutingsMap m: m_routingMap) {
			for(int i = 0; i < 5; i++) {
				final int currentSettingIndex = i;
				final int currentEntryIndex = entryIndex;
				CheckBox ret = m.getCheckBoxForOptionIndex(i);

				if(ret != null) {
					ret.addValueChangeHandler(e -> {
						final int hw = currentEntryIndex;
						final int xx = currentSettingIndex;
						settings.setRoutingAspect(hw, xx, e.getValue());
					});	
				}
			}
			entryIndex++;
		}
	}

	public interface UploadDoneReceiver {
		void onUploadFinished(XMLHttpRequest answer);
	}

	private interface TarUploadedHandler {
		void onTarUploaded(JavaScriptObject buffer);
	}

	private native void loadUpdateFile(NativeEvent evt, TarUploadedHandler receiver) /*-{
		var files = evt.target.files;
		if (files != null && files.length > 0) {
		var file = files[0];
		var reader = new FileReader();
		
		reader.onload = function(e) {
		var zip = reader.result;
		receiver.@com.nonlinearlabs.client.world.overlay.html.setup.Setup.TarUploadedHandler::onTarUploaded(Lcom/google/gwt/core/client/JavaScriptObject;)(zip);
		}
		reader.readAsArrayBuffer(file);
		}
		}-*/;

	public void connectUpdate() {
		DeviceSettingsProvider.get().register(t -> {
			applyPresenter(t);
			return true;
		});

		LocalSettingsProvider.get().register(t -> {
			applyPresenter(t);
			return true;
		});

		DeviceInformationProvider.get().register(t -> {
			applyPresenter(t);
			return true;
		});

		MidiSettingsProvider.get().register(t -> {
			applyPresenter(t);
			return true;
		});

		PresetManagerModel.get().getBanks().onChange(t -> {
			applyBanks(t);
			return true;
		});
	}

	private List<com.nonlinearlabs.client.world.maps.presets.bank.Bank> getBanksSortedByNumber() {
		List<com.nonlinearlabs.client.world.maps.presets.bank.Bank> bb = NonMaps.get().getNonLinearWorld().getPresetManager().getBanks();


		Collections.sort(bb, new Comparator<com.nonlinearlabs.client.world.maps.presets.bank.Bank>() {
			@Override
			public int compare(com.nonlinearlabs.client.world.maps.presets.bank.Bank b2, com.nonlinearlabs.client.world.maps.presets.bank.Bank b1)
			{
				return  b2.getOrderNumber() - b1.getOrderNumber();
			}
		});
		return bb;
	}

	private void applyBanks(Map<String, Bank> map) {
		pcBanks.clear();

		List<com.nonlinearlabs.client.world.maps.presets.bank.Bank> banks = getBanksSortedByNumber();

		pcBanks.addItem("None", "");
		pcBanks.setSelectedIndex(0);
		int index = 1;
		for(com.nonlinearlabs.client.world.maps.presets.bank.Bank b: banks) {
			String name = (index) + "-" + b.getCurrentName();
			pcBanks.addItem(name, b.getUUID());
			if(b.isMidiBank())
				pcBanks.setSelectedIndex(index);
			index++;
		}
	}

	private void fillListboxWithOptions(ListBox box, String[] options) {
		for (String s : options)
			box.addItem(s);
	}

	private void fillRadioButtons(RadioButton on, RadioButton off, String[] options) {
		on.setText(options[0]);
		off.setText(options[1]);
	}

	protected void applyPresenter(DeviceSettings t) {
		velocityCurve.setSelectedIndex(t.velocityCurve.selected);
		aftertouchCurve.setSelectedIndex(t.aftertouchCurve.selected);
		benderCurve.setSelectedIndex(t.benderCurve.selected);

		applyPedalValues(t.pedal1, pedal1Type, pedal1Range, pedal1DisplayString);
		applyPedalValues(t.pedal2, pedal2Type, pedal2Range, pedal2DisplayString);
		applyPedalValues(t.pedal3, pedal3Type, pedal3Range, pedal3DisplayString);
		applyPedalValues(t.pedal4, pedal4Type, pedal4Range, pedal4DisplayString);

		presetGlitchSuppressionOn.setValue(t.presetGlitchSuppression.value);
		presetGlitchSuppressionOff.setValue(!t.presetGlitchSuppression.value);
		editSmoothingTimeRange.setValue(t.editSmoothingTime.sliderPosition);
		editSmoothingTimeDisplayString.setText(t.editSmoothingTime.displayValue);

		syncPartsOn.setValue(t.syncParts.value);
		syncPartsOff.setValue(!t.syncParts.value);

		deviceName.setText(t.deviceName);

		highlightChangedOn.setValue(t.highlightChangedParameters.value);
		highlightChangedOff.setValue(!t.highlightChangedParameters.value);

		tuneReferenceDisplayString.setText(t.tuneReferenceDisplayString);
		tuneReferenceSliderRange.setValue(t.tuneReferenceValue);

		transitionTimeDisplayString.setText(t.transitionTimeDisplayString);
		transitionTimeSliderRange.setValue(t.transitionTimeValue);
	}

	public void applyPedalValues(DeviceSettings.Pedal src, ListBox type, Range slider, Label text) {
		type.setSelectedIndex(src.selected);
		slider.setValue(src.sliderPosition);
		text.setText(src.displayValue);
	}

	private void applyPresenter(LocalSettings t) {
		selectionAutoScroll.setSelectedIndex(t.selectionAutoScroll.selected);
		editParameter.setSelectedIndex(t.editParameter.selected);
		showContextMenusOn.setValue(t.contextMenus.value);
		showContextMenusOff.setValue(!t.contextMenus.value);
		presetDragDropOn.setValue(t.presetDragDrop.value);
		presetDragDropOff.setValue(!t.presetDragDrop.value);
		scalingFactor.setSelectedIndex(t.displayScalingFactor.selected);
		stripeBrightness.setSelectedIndex(t.stripeBrightness.selected);
		bitmapCacheOn.setValue(t.bitmapCache.value);
		bitmapCacheOff.setValue(!t.bitmapCache.value);
		developerOptionsOn.setValue(t.showDeveloperOptions.value);
		developerOptionsOff.setValue(!t.showDeveloperOptions.value);
	}

	private void applyPresenter(DeviceInformation t) {
		freeMemory.setText(t.freeDiscSpace);
		uiVersion.setText(t.uiVersion);
		uiBranch.setText(t.branch);
		uiHead.setText(t.head);
		uiCommits.setText(t.commits);
		dateTime.setText(t.dateTime);
		uiCommitDate.setText(t.commitDate);
		uiUsedRam.setText(t.usedRam);
		uiTotalRam.setText(t.totalRam);
	}

	private void applyPresenter(MidiSettings t) {
		midiReceiveChannel.setSelectedIndex(t.receiveChannel.selected);
		midiReceiveChannelSplit.setSelectedIndex(t.receiveChannelSplit.selected);
		midiSendChannel.setSelectedIndex(t.sendChannel.selected);
		midiSendChannelSplit.setSelectedIndex(t.sendChannelSplit.selected);

		pedal1Mapping.setSelectedIndex(t.pedalMapping1.selected);
		pedal2Mapping.setSelectedIndex(t.pedalMapping2.selected);
		pedal3Mapping.setSelectedIndex(t.pedalMapping3.selected);
		pedal4Mapping.setSelectedIndex(t.pedalMapping4.selected);		
		ribbon1Mapping.setSelectedIndex(t.ribbonMapping1.selected);	
		ribbon2Mapping.setSelectedIndex(t.ribbonMapping2.selected);
		aftertouchMapping.setSelectedIndex(t.aftertouchMapping.selected);
		benderMapping.setSelectedIndex(t.benderMapping.selected);
		highVeloCCOn.setValue(t.highVelocityCC.value);
		highVeloCCOff.setValue(!t.highVelocityCC.value);
		enable14Bit.setValue(t.enable14BitCC.value);
		disable14Bit.setValue(!t.enable14BitCC.value);
		autoStartRecordOn.setValue(t.autoStartRecorder.value);
		autoStartRecordOff.setValue(!t.autoStartRecorder.value);
		
		
		int hwSource = 0;
		for(RoutingsMap routing: m_routingMap) {
			for(int aspect = 0; aspect < RoutingEntry.numAspects; aspect++) {
				final int currentSettingIndex = aspect;
				final int currentHWSource = hwSource;

				CheckBox ret = routing.getCheckBoxForOptionIndex(aspect);

				boolean state = t.routingSetting.routings[currentHWSource].states[currentSettingIndex].value;
				
				if(ret != null) {
					ret.setValue(state);
				}

			}
			hwSource++;
		}
	}

	public void switchPage(Button btn, DivElement page) {
		deviceSettings.getStyle().setDisplay(Display.NONE);
		uiSettings.getStyle().setDisplay(Display.NONE);
		systemInfo.getStyle().setDisplay(Display.NONE);
		about.getStyle().setDisplay(Display.NONE);
		midiSettings.getStyle().setDisplay(Display.NONE);
		flacSettings.getStyle().setDisplay(Display.NONE);

		deviceSettingsButton.getElement().removeClassName("active");
		uiSettingsButton.getElement().removeClassName("active");
		systemInfoButton.getElement().removeClassName("active");
		aboutButton.getElement().removeClassName("active");
		uiMidiButton.getElement().removeClassName("active");
		uiFlacButton.getElement().removeClassName("active");

		btn.getElement().addClassName("active");
		page.getStyle().setDisplay(Display.BLOCK);
	}
}
