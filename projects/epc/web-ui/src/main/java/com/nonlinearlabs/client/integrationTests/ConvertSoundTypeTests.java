package com.nonlinearlabs.client.integrationTests;

import com.google.gwt.user.client.DOM;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.SoundType;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.presets.bank.Bank;

class ConvertSoundTypeTests extends TestWithSteps {

    String bankName = "SoundTypeTests-testbank-" + DOM.createUniqueId();

    public ConvertSoundTypeTests() {
        createBank();
        initSound();
        convertToSingle();
        tweakParameters(VoiceGroup.I, 0.5);
        appendAndAwaitPreset(0);
        convertToLayer();
        assertParameters(VoiceGroup.I, 0.5);
        assertParameters(VoiceGroup.II, 0.5);
        appendAndAwaitPreset(1);
        convertToSingle();
        assertParameters(VoiceGroup.I, 0.5);
        tweakParameters(VoiceGroup.I, 0.75);
        appendAndAwaitPreset(2);
        convertToSplit();
        assertParameters(VoiceGroup.I, 0.75);
        assertParameters(VoiceGroup.II, 0.75);
        appendAndAwaitPreset(3);
        convertToSingle();
        assertParameters(VoiceGroup.I, 0.75);
        appendAndAwaitPreset(4);

        deleteBank();

        start();
    }

    private void tweakParameters(VoiceGroup vg, double offset) {
        addStep(() -> {
            setParameter(19, vg, offset + 0.00);
            setParameter(295, vg, offset + 0.01);
            setParameter(21, vg, offset + 0.02);
            setParameter(23, vg, offset + 0.03);
            setParameter(25, vg, offset + 0.04);
            setParameter(27, vg, offset + 0.05);
            setParameter(29, vg, offset + 0.06);
            setParameter(31, vg, offset + 0.07);
            setParameter(33, vg, offset + 0.08);
            setParameter(34, vg, offset + 0.09);
            setParameter(35, vg, offset + 0.10);
        }, () -> true);

        setStepName("Tweak Parameters " + vg + " offset " + offset);
    }

    private void assertParameters(VoiceGroup vg, double offset) {
        addStep(() -> {
            assertParameter(19, vg, offset + 0.00);
            assertParameter(295, vg, offset + 0.01);
            assertParameter(21, vg, offset + 0.02);
            assertParameter(23, vg, offset + 0.03);
            assertParameter(25, vg, offset + 0.04);
            assertParameter(27, vg, offset + 0.05);
            assertParameter(29, vg, offset + 0.06);
            assertParameter(31, vg, offset + 0.07);
            assertParameter(33, vg, offset + 0.08);
            assertParameter(34, vg, offset + 0.09);
            assertParameter(35, vg, offset + 0.10);
        }, () -> true);

        setStepName("Assert Parameters " + vg + " offset " + offset);
    }

    private void assertParameter(int i, VoiceGroup vg, double d) {
        double v = EditBufferModel.get().getParameter(new ParameterId(i, vg)).value.value.getValue();
        assert (Math.abs(v - d) < 0.005);
    }

    private void setParameter(int i, VoiceGroup vg, double d) {
        EditBufferUseCases uc = EditBufferUseCases.get();
        uc.setParameterValue(new ParameterId(i, vg), d, false);
    }

    private void initSound() {
        addStep(() -> {
            EditBufferUseCases.get().initializeSound();
        }, () -> true);
        setStepName("InitSound");
    }

    private void convertToSplit() {
        addStep(() -> {
            EditBufferUseCases.get().convertToSplitSound();
        }, () -> {
            return EditBufferModel.get().soundType.getValue() == SoundType.Split;
        });
        setStepName("convertToSplit");
    }

    private void convertToLayer() {
        addStep(() -> {
            EditBufferUseCases.get().convertToLayerSound();
        }, () -> {
            return EditBufferModel.get().soundType.getValue() == SoundType.Layer;
        });
        setStepName("convertToLayer");
    }

    private void convertToSingle() {
        addStep(() -> {
            EditBufferUseCases.get().convertToSingleSound();
        }, () -> {
            return EditBufferModel.get().soundType.getValue() == SoundType.Single;
        });
        setStepName("convertToSingle");
    }

    private void createBank() {
        addStep(() -> {
            ServerProxy p = NonMaps.get().getServerProxy();
            p.newBank(bankName, new NonPosition());
        }, () -> {
            return findBank() != null;
        });
        setStepName("createBank");
    }

    private void appendAndAwaitPreset(int n) {
        addStep(() -> {
            ServerProxy p = NonMaps.get().getServerProxy();
            p.appendEditBuffer(findBank());
        }, () -> {
            return findBank().getPreset(n) != null;
        });

        setStepName("appendAndAwaitPreset " + n);
    }

    private Bank findBank() {
        PresetManager pm = NonMaps.get().getNonLinearWorld().getPresetManager();
        for (Bank b : pm.getBanks())
            if (b.getCurrentName() == bankName)
                return b;

        return null;
    }

    private void deleteBank() {
        addStep(250, () -> {
            Bank b = findBank();
            ServerProxy p = NonMaps.get().getServerProxy();
            p.deleteBank(b.getUUID());
        }, () -> {
            return findBank() == null;
        });
        setStepName("delete Bank");
    }

}