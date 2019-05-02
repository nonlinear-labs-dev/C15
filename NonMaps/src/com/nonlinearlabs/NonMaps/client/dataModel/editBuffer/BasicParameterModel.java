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
		
	private static int roundCompare(double value) {
	    BigDecimal bd = new BigDecimal(value);
	    bd = bd.setScale(1, RoundingMode.HALF_UP);
	    return (int)bd.doubleValue();
	}
	
	public boolean isValueChanged() {
		int compareDenominator = value.metaData.fineDenominator.getValue();
		int roundedVal = roundCompare(value.value.getValue() * compareDenominator);
		int roundedOgVal = roundCompare(originalValue.getValue() * compareDenominator);
		return roundedVal != roundedOgVal;
	}
	
	public boolean isChanged() {
		return isValueChanged();
	}

	public Updater getUpdater(Node c) {
		return new ParameterUpdater(c, this);
	}

}
