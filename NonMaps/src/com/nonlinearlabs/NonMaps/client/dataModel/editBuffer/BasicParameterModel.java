package com.nonlinearlabs.NonMaps.client.dataModel.editBuffer;

import java.math.BigDecimal;
import java.math.RoundingMode;

import com.google.gwt.core.client.GWT;
import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.NonMaps.client.dataModel.DoubleDataModelEntity;
import com.nonlinearlabs.NonMaps.client.dataModel.Notifier;
import com.nonlinearlabs.NonMaps.client.dataModel.StringDataModelEntity;
import com.nonlinearlabs.NonMaps.client.dataModel.Updater;
import com.nonlinearlabs.NonMaps.client.dataModel.ValueDataModelEntity;

public class BasicParameterModel extends Notifier<BasicParameterModel> {

	public ValueDataModelEntity value = new ValueDataModelEntity();
	public StringDataModelEntity shortName = new StringDataModelEntity();
	public StringDataModelEntity longName = new StringDataModelEntity();
	public DoubleDataModelEntity originalValue = new DoubleDataModelEntity();

	public BasicParameterModel() {
		value.onChange(e -> notifyChanges());
		shortName.onChange(e -> notifyChanges());
		longName.onChange(e -> notifyChanges());
		originalValue.onChange(e -> notifyChanges());
	}

	@Override
	public BasicParameterModel getValue() {
		return this;
	}
	
	private double roundForCompare(double val) {
		int tmp = (int) (val * 100000);
		return tmp / 100000.0;
	}
	
	public boolean isValueChanged() {
		double og = roundForCompare(originalValue.getValue()); 
		double val = roundForCompare(value.value.getValue()); 
		
		int compareDenominator = value.metaData.fineDenominator.getValue();
		int roundedVal = (int) (val * compareDenominator);
		int roundedOgVal = (int) (og * compareDenominator);

		return roundedVal != roundedOgVal;
	}
	
	public boolean isChanged() {
		return isValueChanged();
	}

	public Updater getUpdater(Node c) {
		return new ParameterUpdater(c, this);
	}

}
