package com.nonlinearlabs.client.presenters;

public class DeviceSettings {
	static public class VelocityCurve {
		static public String[] options = { "Very Soft", "Soft", "Normal", "Hard", "Very Hard" };
		public int selected = 2;
	}

	static public class AftertouchCurve {
		static public String[] options = { "Soft", "Normal", "Hard" };
		public int selected = 1;
	}

	static public class BenderCurve {
		static public String[] options = { "Soft", "Normal", "Hard" };
		public int selected = 1;
	}

	static public class Pedal {
		public String displayValue = "---";
		public double sliderPosition = 0;
		public int selected = 0;
	}

	static public class PresetGlitchSuppression {
		static public String[] options = { "On", "Off" };
		public boolean value = false;
	}

	public class EditSmoothingTime {
		public String displayValue = "---";
		public double sliderPosition = 0;
	}

	static public class HighlightChanged {
		static public String[] options = { "On", "Off" };
		public boolean value = false;
	}

	static public class SyncPartsAcrossUI {
		static public String[] options = {"On", "Off"};
		public boolean value = false;
	}

	public VelocityCurve velocityCurve = new VelocityCurve();
	public AftertouchCurve aftertouchCurve = new AftertouchCurve();
	public BenderCurve benderCurve = new BenderCurve();
	public Pedal pedal1 = new Pedal();
	public Pedal pedal2 = new Pedal();
	public Pedal pedal3 = new Pedal();
	public Pedal pedal4 = new Pedal();
	public PresetGlitchSuppression presetGlitchSuppression = new PresetGlitchSuppression();
	public EditSmoothingTime editSmoothingTime = new EditSmoothingTime();
	public String deviceName = "";
	public HighlightChanged highlightChangedParameters = new HighlightChanged();
	public SyncPartsAcrossUI syncParts = new SyncPartsAcrossUI();

	public double randomizeAmountValue = 0;
	public String randomizeAmountDisplayString = "";

	public double transitionTimeValue = 0;
	public String transitionTimeDisplayString = "";

	public double tuneReferenceValue = 0;
	public String tuneReferenceDisplayString = "";

	public boolean syncSplitsEnabled = false;
}
