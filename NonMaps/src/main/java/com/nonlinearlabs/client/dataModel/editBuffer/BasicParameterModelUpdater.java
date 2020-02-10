package com.nonlinearlabs.client.dataModel.editBuffer;

import com.google.gwt.xml.client.Node;
import com.nonlinearlabs.client.dataModel.Updater;
import com.nonlinearlabs.client.dataModel.ValueUpdater;

public class BasicParameterModelUpdater extends Updater {

	protected BasicParameterModel target;

	public BasicParameterModelUpdater(Node c, BasicParameterModel p) {
		super(c);
		target = p;
	}

	public void doUpdate() {
		String shortName = getChildText(root, "short-name");
		String longName = getChildText(root, "long-name");
		String info = getChildText(root, "info-text");
		
		if (!shortName.isEmpty())
			target.shortName.setValue(shortName);

		if (!longName.isEmpty())
			target.longName.setValue(longName);

		if(!info.isEmpty())
			target.info.setValue(info);

		target.locked.setValue(getAttributeValue(root, "locked").equals("1"));

		ValueUpdater vu = new ValueUpdater(root, target.value);
		vu.doUpdate();
	}

}
