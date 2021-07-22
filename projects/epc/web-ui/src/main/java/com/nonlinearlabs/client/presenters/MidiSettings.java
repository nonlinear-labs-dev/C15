package com.nonlinearlabs.client.presenters;
import com.nonlinearlabs.client.dataModel.RoutingAspectsSetting;

public class MidiSettings {

	static public class VelocityCurve {
		static public String[] options = { "Very Soft", "Soft", "Normal", "Hard", "Very Hard" };
		public int selected = 2;
	}

	static public class AftertouchCurve {
		static public String[] options = { "Soft", "Normal", "Hard" };
		public int selected = 1;
	}

    static public class SendChannel {
        static public String[] options = {"None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};
        public int selected = 0;
    }

    static public class SendChannelSplit {
        static public String[] options = {"None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "Common"};
        public int selected = 0;
    }
    
    static public class ReceiveChannel {
        static public String[] options = {"None", "Omni", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};
        public int selected = 0;
    }


    static public class ReceiveChannelSplit {
        static public String[] options = {"None", "Omni", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "Common"};
        public int selected = 0;
    }


	static public class OnOffOption {
		static public String[] options = { "On", "Off" };
		public boolean value = false;
    }

    static public class PedalMapping {
        static public String[] options = { 
            "None", "CC 01 (LSB: CC 33)", "CC 02 (LSB: CC 34)", "CC 03 (LSB: CC 35)", "CC 04 (LSB: CC 36)", "CC 05 (LSB: CC 37)",
            "CC 06 (LSB: CC 38)", "CC 07 (LSB: CC 39)", "CC 08 (LSB: CC 40)", "CC 09 (LSB: CC 41)", "CC 10 (LSB: CC 42)",
            "CC 11 (LSB: CC 43)", "CC 12 (LSB: CC 44)", "CC 13 (LSB: CC 45)", "CC 14 (LSB: CC 46)", "CC 15 (LSB: CC 47)",
            "CC 16 (LSB: CC 48)", "CC 17 (LSB: CC 49)", "CC 18 (LSB: CC 50)", "CC 19 (LSB: CC 51)", "CC 20 (LSB: CC 52)",
            "CC 21 (LSB: CC 53)", "CC 22 (LSB: CC 54)", "CC 23 (LSB: CC 55)", "CC 24 (LSB: CC 56)", "CC 25 (LSB: CC 57)",
            "CC 26 (LSB: CC 58)", "CC 27 (LSB: CC 59)", "CC 28 (LSB: CC 60)", "CC 29 (LSB: CC 61)", "CC 30 (LSB: CC 62)",
            "CC 31 (LSB: CC 63)", "CC 64 (switching)",  "CC 65 (switching)",  "CC 66 (switching)",  "CC 67 (switching)",
            "CC 68 (switching)",  "CC 69 (switching)"
        };
        public int selected = 0;
    }

    static public class RibbonMapping {
        static public String[] options = {
            "None", "CC 01 (LSB: CC 33)", "CC 02 (LSB: CC 34)", "CC 03 (LSB: CC 35)", "CC 04 (LSB: CC 36)", "CC 05 (LSB: CC 37)",
            "CC 06 (LSB: CC 38)", "CC 07 (LSB: CC 39)", "CC 08 (LSB: CC 40)", "CC 09 (LSB: CC 41)", "CC 10 (LSB: CC 42)",
            "CC 11 (LSB: CC 43)", "CC 12 (LSB: CC 44)", "CC 13 (LSB: CC 45)", "CC 14 (LSB: CC 46)", "CC 15 (LSB: CC 47)",
            "CC 16 (LSB: CC 48)", "CC 17 (LSB: CC 49)", "CC 18 (LSB: CC 50)", "CC 19 (LSB: CC 51)", "CC 20 (LSB: CC 52)",
            "CC 21 (LSB: CC 53)", "CC 22 (LSB: CC 54)", "CC 23 (LSB: CC 55)", "CC 24 (LSB: CC 56)", "CC 25 (LSB: CC 57)",
            "CC 26 (LSB: CC 58)", "CC 27 (LSB: CC 59)", "CC 28 (LSB: CC 60)", "CC 29 (LSB: CC 61)", "CC 30 (LSB: CC 62)",
            "CC 31 (LSB: CC 63)" 
        };
        public int selected = 0;
    }

    static public class BenderMapping {
        static public String[] options = {
            "None", "Pitchbend", "CC 01 (LSB: CC 33)", "CC 02 (LSB: CC 34)", "CC 03 (LSB: CC 35)", "CC 04 (LSB: CC 36)",
            "CC 05 (LSB: CC 37)", "CC 06 (LSB: CC 38)", "CC 07 (LSB: CC 39)", "CC 08 (LSB: CC 40)", "CC 09 (LSB: CC 41)",
            "CC 10 (LSB: CC 42)", "CC 11 (LSB: CC 43)", "CC 12 (LSB: CC 44)", "CC 13 (LSB: CC 45)", "CC 14 (LSB: CC 46)",
            "CC 15 (LSB: CC 47)", "CC 16 (LSB: CC 48)", "CC 17 (LSB: CC 49)", "CC 18 (LSB: CC 50)", "CC 19 (LSB: CC 51)",
            "CC 20 (LSB: CC 52)", "CC 21 (LSB: CC 53)", "CC 22 (LSB: CC 54)", "CC 23 (LSB: CC 55)", "CC 24 (LSB: CC 56)",
            "CC 25 (LSB: CC 57)", "CC 26 (LSB: CC 58)", "CC 27 (LSB: CC 59)", "CC 28 (LSB: CC 60)", "CC 29 (LSB: CC 61)",
            "CC 30 (LSB: CC 62)", "CC 31 (LSB: CC 63)"
        };
        public int selected = 0;
    }

    static public class AftertouchMapping {
        static public String[] options = {
            "None", "Channel Pressure",   "CC 01 (LSB: CC 33)", "CC 02 (LSB: CC 34)", "CC 03 (LSB: CC 35)", "CC 04 (LSB: CC 36)",
            "CC 05 (LSB: CC 37)", "CC 06 (LSB: CC 38)", "CC 07 (LSB: CC 39)", "CC 08 (LSB: CC 40)", "CC 09 (LSB: CC 41)",
            "CC 10 (LSB: CC 42)", "CC 11 (LSB: CC 43)", "CC 12 (LSB: CC 44)", "CC 13 (LSB: CC 45)", "CC 14 (LSB: CC 46)",
            "CC 15 (LSB: CC 47)", "CC 16 (LSB: CC 48)", "CC 17 (LSB: CC 49)", "CC 18 (LSB: CC 50)", "CC 19 (LSB: CC 51)",
            "CC 20 (LSB: CC 52)", "CC 21 (LSB: CC 53)", "CC 22 (LSB: CC 54)", "CC 23 (LSB: CC 55)", "CC 24 (LSB: CC 56)",
            "CC 25 (LSB: CC 57)", "CC 26 (LSB: CC 58)", "CC 27 (LSB: CC 59)", "CC 28 (LSB: CC 60)", "CC 29 (LSB: CC 61)",
            "CC 30 (LSB: CC 62)", "CC 31 (LSB: CC 63)", "Pitchbend up", "Pitchbend down"
        };
        public int selected = 0;
    }

    static public class RoutingEntry {
        public static final int numAspects = 5;
        public RoutingEntry() {
            for(int i = 0; i < numAspects; i++) {
                states[i] = new OnOffOption();
            }
        }

        public OnOffOption states[] = new OnOffOption[numAspects];
    }

    static public class RoutingSetting {
        public static final int numRoutings = 10;
        RoutingSetting() {
            for(int i = 0; i < numRoutings; i++) {
                routings[i] = new RoutingEntry();
            }
        }
        public RoutingEntry routings[] = new RoutingEntry[numRoutings]; 
    }

    //Global
    public OnOffOption globalLocalEnable = new OnOffOption();

    //Channels
    public SendChannel sendChannel = new SendChannel();
    public SendChannelSplit sendChannelSplit = new SendChannelSplit();
    public ReceiveChannel receiveChannel = new ReceiveChannel();
    public ReceiveChannelSplit receiveChannelSplit = new ReceiveChannelSplit();

    //Unused
    public VelocityCurve receiveVelocityCurve = new VelocityCurve();
    public AftertouchCurve receiveAftertouchCurve = new AftertouchCurve();

    //Mapping
    public PedalMapping pedalMapping1 = new PedalMapping();
    public PedalMapping pedalMapping2 = new PedalMapping();
    public PedalMapping pedalMapping3 = new PedalMapping();
    public PedalMapping pedalMapping4 = new PedalMapping();
    public RibbonMapping ribbonMapping1 = new RibbonMapping();
    public RibbonMapping ribbonMapping2 = new RibbonMapping();
    public BenderMapping benderMapping = new BenderMapping();
    public AftertouchMapping aftertouchMapping = new AftertouchMapping();
    public OnOffOption highVelocityCC = new OnOffOption();
    public OnOffOption enable14BitCC = new OnOffOption();

    //Routing
    public RoutingSetting routingSetting = new RoutingSetting();

    //FLAC
    public OnOffOption autoStartRecorder = new OnOffOption();

    public static RoutingSetting convert(RoutingAspectsSetting s) {
        RoutingSetting ret = new RoutingSetting();
        for(int routing = 0; routing < RoutingSetting.numRoutings; routing++) {
            for(int aspect = 0; aspect < RoutingEntry.numAspects; aspect++) {
                boolean state = s.m_data[routing][aspect];
                ret.routings[routing].states[aspect].value = state;
            }
        }
        return ret;
    }
}
