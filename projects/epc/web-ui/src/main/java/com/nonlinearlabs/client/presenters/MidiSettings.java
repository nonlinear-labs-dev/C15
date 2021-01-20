package com.nonlinearlabs.client.presenters;

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
        static public String[] options = {"None", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "Follow"};
        public int selected = 0;
    }

    
    static public class ReceiveChannel {
        static public String[] options = {"None", "Omni", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};
        public int selected = 0;
    }


    static public class ReceiveChannelSplit {
        static public String[] options = {"None", "Omni", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "Follow"};
        public int selected = 0;
    }


	static public class OnOffOption {
		static public String[] options = { "On", "Off" };
		public boolean value = false;
    }
    

    //Local
    public OnOffOption localControllers = new OnOffOption();
    public OnOffOption localNotes = new OnOffOption();
    public OnOffOption localProgramChanges = new OnOffOption();

    //Send
    public SendChannel sendChannel = new SendChannel();
    public SendChannelSplit sendChannelSplit = new SendChannelSplit();
    public OnOffOption sendControllers = new OnOffOption();
    public OnOffOption sendProgramChanges = new OnOffOption();
    public OnOffOption sendNotes = new OnOffOption();

    //Receive
    public ReceiveChannel receiveChannel = new ReceiveChannel();
    public ReceiveChannelSplit receiveChannelSplit = new ReceiveChannelSplit();
    public OnOffOption receiveProgramChanges = new OnOffOption();
    public OnOffOption receiveNotes = new OnOffOption();
    public OnOffOption receiveControllers = new OnOffOption();
    public VelocityCurve receiveVelocityCurve = new VelocityCurve();
    public AftertouchCurve receiveAftertouchCurve = new AftertouchCurve();
}
