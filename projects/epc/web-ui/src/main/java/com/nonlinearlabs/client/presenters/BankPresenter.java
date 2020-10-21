package com.nonlinearlabs.client.presenters;

import java.util.ArrayList;

public class BankPresenter {
    public String name = "";
    public String comment = "";
    public String uuid = "";

    public String dateOfExportFile = "";
    public String dateOfImportFile = "";
    public String dateOfLastChange = "";
    public String nameOfExportFile = "";
    public String nameOfImportFile = "";
    public String importExportState = "";

    public ArrayList<String> presets = new ArrayList<String>();
    public boolean isSelected = false;
    public int orderNumber = 0;
    public boolean canSelectNextPreset = false;
    public boolean canSelectPrevPreset = false;

}