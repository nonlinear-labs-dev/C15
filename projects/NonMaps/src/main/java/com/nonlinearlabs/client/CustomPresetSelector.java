package com.nonlinearlabs.client;

import com.nonlinearlabs.client.world.maps.presets.bank.Bank;
import com.nonlinearlabs.client.world.maps.presets.bank.preset.Preset;

public interface CustomPresetSelector {
    public void updateUI();

    public Bank getSelectedBank();

    public Preset getSelectedPreset();

    public Preset getOriginalPreset();

    public boolean canNext();

    public boolean canPrev();

    public void setSelectedBank(Bank findBank);

    public void selectPreviousPreset();

    public void selectNextPreset();

    public void selectPreviousBank();

    public  void selectNextBank();

    public  boolean canSelectPreviousBank();

    public  boolean canSelectNextBank();

    public  boolean isOriginalPreset(Preset mapsPreset);

	public  void setSelectedPreset(Preset mapsPreset) ;
}
