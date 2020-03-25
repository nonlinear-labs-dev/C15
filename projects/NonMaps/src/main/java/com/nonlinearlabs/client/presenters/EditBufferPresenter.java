package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;

public class EditBufferPresenter {
    public boolean allParametersLocked = false;
    public boolean isAnyParameterLocked = false;
    public boolean isAnyParameterChanged = false;
    public boolean isAnyScaleOffsetParameterNotDefault = false;
    public String loadedPresetUUID = "";
    public ParameterPresenter selectedParameter;
    public String voiceGroup = "";
    public RGB voiceGroupIndicationColor = RGBA.transparent();
    public VoiceGroup voiceGroupEnum = VoiceGroup.I;

    public RGB voiceGroupI_ForegroundColor = RGBA.transparent();
    public RGB voiceGroupII_ForegroundColor = RGBA.transparent();
    public RGB voiceGroupI_BackgroundColor = RGBA.transparent();
    public RGB voiceGroupII_BackgroundColor = RGBA.transparent();

    public String currentPartName;
    public SoundType soundType;
}