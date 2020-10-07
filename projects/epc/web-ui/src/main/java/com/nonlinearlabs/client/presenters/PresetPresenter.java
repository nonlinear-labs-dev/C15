package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.presetManager.Preset.Color;

public class PresetPresenter {
    public String bankUuid = "";
    public String paddedNumber = "000";
    public int rawNumber = 0;
    public String name = "n/a";
    public boolean loaded = false;
    public boolean selected = false;
    public boolean isLastPresetInBank = false;
    public SoundType type = SoundType.Single;
    public boolean isDual = false;
    public String deviceName = "";
    public String softwareVersion = "";
    public String storeTime = "";
    public String comment = "";
    public Color color = Color.none;
    public Integer storeCounter = 0;
    public String bankName = "";

}