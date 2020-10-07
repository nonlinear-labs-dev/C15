package com.nonlinearlabs.client.dataModel.presetManager;

import com.nonlinearlabs.client.dataModel.EnumDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;

public class Preset extends Doomable {
	public static enum Color {
		green, blue, yellow, orange, purple, red, none;

		public RGB toRGB() {
			switch (this) {
				case green:
					return new RGB(82, 173, 69);
				case blue:
					return new RGB(75, 105, 213);
				case yellow:
					return new RGB(204, 197, 54);
				case orange:
					return new RGB(221, 149, 55);
				case purple:
					return new RGB(183, 95, 174);
				case red:
					return new RGB(207, 43, 59);
				case none:
				default:
					return new RGBA(0, 0, 0, 0);
			}
		}

		static public Color toEnum(String s) {
			if (s == "")
				return none;
			else
				return Color.valueOf(s);
		}
	}

	public StringDataModelEntity name = new StringDataModelEntity();
	public StringDataModelEntity uuid = new StringDataModelEntity();
	public StringDataModelEntity bankUuid = new StringDataModelEntity();
	public IntegerDataModelEntity number = new IntegerDataModelEntity();
	public EnumDataModelEntity<SoundType> type = new EnumDataModelEntity<SoundType>(SoundType.class, SoundType.Single);

	public StringDataModelEntity deviceName = new StringDataModelEntity();
	public StringDataModelEntity softwareVersion = new StringDataModelEntity();
	public StringDataModelEntity storeTime = new StringDataModelEntity();
	public StringDataModelEntity comment = new StringDataModelEntity();
	public IntegerDataModelEntity storeCounter = new IntegerDataModelEntity();
	public EnumDataModelEntity<Color> color = new EnumDataModelEntity<Color>(Color.class, Color.none);
}