package com.nonlinearlabs.client.dataModel.presetManager;

import java.util.ArrayList;

import com.nonlinearlabs.client.dataModel.BooleanDataModelEntity;
import com.nonlinearlabs.client.dataModel.DoubleDataModelEntity;
import com.nonlinearlabs.client.dataModel.IntegerDataModelEntity;
import com.nonlinearlabs.client.dataModel.StringDataModelEntity;

public class Bank extends Doomable {

	public static class Position {
		public Position(String bank) {
			this.bank = bank;
		}

		public String bank;
	}

	public static class AbsolutePosition extends Position {
		AbsolutePosition(String bank, double x, double y) {
			super(bank);
			this.x = x;
			this.y = y;
		}

		public double x;
		public double y;
	}

	public static class RelativePosition extends AbsolutePosition {
		RelativePosition(String bank, String attachedTo, String direction, double x, double y) {
			super(bank, x, y);
			this.attachedTo = attachedTo;
			this.direction = direction;
		}

		public String attachedTo;
		public String direction;
	}

	public PresetListDataModelEntity presets = new PresetListDataModelEntity(new ArrayList<String>());
	public StringDataModelEntity name = new StringDataModelEntity();
	public IntegerDataModelEntity orderNumber = new IntegerDataModelEntity();
	public StringDataModelEntity uuid = new StringDataModelEntity();
	public BooleanDataModelEntity isMidiBank = new BooleanDataModelEntity();
	public IntegerDataModelEntity orderNumber = new IntegerDataModelEntity();
	public StringDataModelEntity selectedPreset = new StringDataModelEntity();
	public DoubleDataModelEntity x = new DoubleDataModelEntity();
	public DoubleDataModelEntity y = new DoubleDataModelEntity();
	public StringDataModelEntity attachedToBank = new StringDataModelEntity();
	public StringDataModelEntity attachDirection = new StringDataModelEntity();

	public StringDataModelEntity comment = new StringDataModelEntity();
	public StringDataModelEntity dateOfExportFile = new StringDataModelEntity();
	public StringDataModelEntity dateOfImportFile = new StringDataModelEntity();
	public StringDataModelEntity nameOfExportFile = new StringDataModelEntity();
	public StringDataModelEntity nameOfImportFile = new StringDataModelEntity();
	public StringDataModelEntity dateOfLastChange = new StringDataModelEntity();
	public StringDataModelEntity importExportState = new StringDataModelEntity();
}
