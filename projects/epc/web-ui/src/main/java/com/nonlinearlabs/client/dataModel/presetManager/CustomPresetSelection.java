package com.nonlinearlabs.client.dataModel.presetManager;

public interface CustomPresetSelection {
    public String getSelectedBank();

    public String getSelectedPreset();

    public String getOriginalPreset();

    public boolean canNext();

    public boolean canPrev();

    public void setSelectedBank(String findBank);

    public void selectPreviousPreset();

    public void selectNextPreset();

    public void selectPreviousBank();

    public void selectNextBank();

    public boolean canSelectPreviousBank();

    public boolean canSelectNextBank();

    public boolean isOriginalPreset(String mapsPreset);

    public void setSelectedPreset(String mapsPreset);
}
