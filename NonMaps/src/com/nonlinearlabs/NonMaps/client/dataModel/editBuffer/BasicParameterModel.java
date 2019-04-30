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

	int getDecimalPlaces(Double d) {
		if(d.toString().contains("\\.")) {
			String[] splitter = d.toString().split("\\.");
			return splitter[1].length() + 1;
		}
		return 0;
	}	
	
	double scale(double v, int scale) {
		BigDecimal foo = new BigDecimal(v).setScale(scale, RoundingMode.HALF_UP);
		return foo.doubleValue();
	}
	
	public boolean isValueChanged() {
		double og = originalValue.getValue(); 
		double val = value.value.getValue(); 
		
		int valDigits = Math.max(getDecimalPlaces(val), 5);
				
		og = scale(og, valDigits);
		val = scale(val, valDigits);
		
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
