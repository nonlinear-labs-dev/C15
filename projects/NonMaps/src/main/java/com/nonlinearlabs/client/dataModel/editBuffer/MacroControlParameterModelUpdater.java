package com.nonlinearlabs.client.dataModel.editBuffer;

import com.google.gwt.xml.client.Node;

public class MacroControlParameterModelUpdater extends BasicParameterModelUpdater {

    public MacroControlParameterModelUpdater(Node c, MacroControlParameterModel p) {
        super(c, p);
    }

    @Override
    public void doUpdate() {
        super.doUpdate();

        MacroControlParameterModel m = (MacroControlParameterModel)target;
        m.givenName.setValue(getChildText(root, "givenName"));
        m.info.setValue(getChildText(root, "info"));
        m.ogInfo.setValue(getChildText(root, "og-info"));
        m.ogGivenName.setValue(getChildText(root, "og-name"));
    }

}
