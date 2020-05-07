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

		static public String[] options = { "Pot Tip Active", "Pot Ring Active", "Pot Tip Active Reverse", "Pot Ring Active Reverse", "Resistor", "Resitor Reversed", 
										   "Switch Closing", "Switch Opening", "CV 0..5V", "CV 0..5V Auto Range", "OFF", "Boss EV 30", "Doepfer FP 5", "Fractal EV 2",
										   "Korg DS1H", "Korg EXP2", "Lead Foot LFX1", "MAudio EXP", "Moog EP3", "Roland DP10", "Roland EV5", "Yamaha FC3A", "Yamaha FC7" };
		public int selected = 0;
	}

	static public class PresetGlitchSurpression {
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

	public VelocityCurve velocityCurve = new VelocityCurve();
	public AftertouchCurve aftertouchCurve = new AftertouchCurve();
	public BenderCurve benderCurve = new BenderCurve();
	public Pedal pedal1 = new Pedal();
	public Pedal pedal2 = new Pedal();
	public Pedal pedal3 = new Pedal();
	public Pedal pedal4 = new Pedal();
	public PresetGlitchSurpression presetGlitchSurpession = new PresetGlitchSurpression();
	public EditSmoothingTime editSmoothingTime = new EditSmoothingTime();
	public String deviceName = "";
	public HighlightChanged highlightChangedParameters = new HighlightChanged();

	public double randomizeAmountValue = 0;
	public String randomizeAmountDisplayString = "";

	public double transitionTimeValue = 0;
	public String transitionTimeDisplayString = "";

	public double tuneReferenceValue = 0;
	public String tuneReferenceDisplayString = "";
}
