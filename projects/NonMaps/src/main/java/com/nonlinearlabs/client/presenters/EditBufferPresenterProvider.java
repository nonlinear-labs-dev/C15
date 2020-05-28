package com.nonlinearlabs.client.presenters;

import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.ScheduledCommand;
import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterFactory;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.ScaleOffsetParameterModel;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.RGBA;

public class EditBufferPresenterProvider extends Notifier<EditBufferPresenter> {
    private static EditBufferPresenterProvider theProvider = new EditBufferPresenterProvider();
    private EditBufferPresenter presenter = new EditBufferPresenter();
    private boolean bruteForceScheduled = false;
    private int allParameterSubscription = 0;
    private int selectedParameterSubscription = 0;

    private EditBufferPresenterProvider() {
        EditBufferModel model = EditBufferModel.get();

        model.voiceGroup.onChange(v -> {
            monitorSelectedParameter();

            presenter.voiceGroupEnum = v;

            switch (v) {
                case Global:
                    presenter.voiceGroup = "";
                    presenter.voiceGroupIndicationColor = RGBA.transparent();
                    presenter.voiceGroupI_ForegroundColor = RGBA.transparent();
                    presenter.voiceGroupII_ForegroundColor = RGBA.transparent();
                    presenter.voiceGroupI_BackgroundColor = RGBA.transparent();
                    presenter.voiceGroupII_BackgroundColor = RGBA.transparent();
                    break;

                case I:
                    presenter.voiceGroup = "\uE071";
                    presenter.voiceGroupIndicationColor = new RGB(0x26, 0xb0, 0xff);
                    presenter.voiceGroupI_ForegroundColor = new RGB(0x26, 0xb0, 0xff);
                    presenter.voiceGroupII_ForegroundColor = new Gray(0x08);
                    presenter.voiceGroupI_BackgroundColor = new Gray(0x66);
                    presenter.voiceGroupII_BackgroundColor = new Gray(0x4c);
                    break;

                case II:
                    presenter.voiceGroup = "\uE072";
                    presenter.voiceGroupIndicationColor = new RGB(0xff, 0x99, 0x33);
                    presenter.voiceGroupI_ForegroundColor = new Gray(0x08);
                    presenter.voiceGroupII_ForegroundColor = new RGB(0xff, 0x99, 0x33);
                    presenter.voiceGroupI_BackgroundColor = new Gray(0x4c);
                    presenter.voiceGroupII_BackgroundColor = new Gray(0x66);
                    break;
            }

            presenter.currentPartName = model.getPresetNameOfVoiceGroup(v);
            notifyChanges();
            return true;
        });

        model.onChange(v -> {
            monitorAllParameters();
            return true;
        });

        model.loadedPreset.onChange(v -> {
            presenter.loadedPresetUUID = v;
            notifyChanges();
            return true;
        });

        model.soundType.onChange(v -> {
            presenter.soundType = v;
            notifyChanges();
            return true;
        });
    }

    private void monitorAllParameters() {
        int localSubscribtion = ++allParameterSubscription;

        for (VoiceGroup g : VoiceGroup.values()) {
            for (BasicParameterModel param : EditBufferModel.get().byVoiceGroup[g.ordinal()].parameters.values()) {
                ParameterPresenterProviders.get().register(param.id, v -> {
                    if (localSubscribtion == allParameterSubscription) {
                        scheduleBruteForce();
                        return true;
                    }
                    return false;
                });
            }
        }
    }

    private void scheduleBruteForce() {
        if (!bruteForceScheduled) {
            bruteForceScheduled = true;

            Scheduler.get().scheduleDeferred(new ScheduledCommand() {

                @Override
                public void execute() {
                    bruteForceScheduled = false;
                    bruteForce();
                }
            });
        }
    }

    private void bruteForce() {
        boolean anyScaleNotDef = isAnyScaleOffsetParameterNotDefault();
        if (presenter.isAnyScaleOffsetParameterNotDefault != anyScaleNotDef) {
            presenter.isAnyScaleOffsetParameterNotDefault = anyScaleNotDef;
            notifyChanges();
        }

        boolean anyChanged = isAnyParameterChanged();
        if (presenter.isAnyParameterChanged != anyChanged) {
            presenter.isAnyParameterChanged = anyChanged;
            notifyChanges();
        }

        boolean anyLocked = isAnyParameterLocked();
        if(anyLocked != presenter.isAnyParameterLocked) {
            presenter.isAnyParameterLocked = anyLocked;
            notifyChanges();
        }

        boolean allLocked = areAllParametersLocked();
        if(allLocked != presenter.allParametersLocked) {
            presenter.allParametersLocked = allLocked;
            notifyChanges();
        }
    }

    private boolean isAnyParameterLocked() {
        for (VoiceGroup g : VoiceGroup.values()) {
            for (BasicParameterModel param : EditBufferModel.get().byVoiceGroup[g.ordinal()].parameters.values()) {
                if (param.isLocked())
                    return true;
            }
        }

        return false;
    }

    public boolean areAllParametersLocked() {
        for (VoiceGroup g : VoiceGroup.values()) {
            for (BasicParameterModel param : EditBufferModel.get().byVoiceGroup[g.ordinal()].parameters.values()) {
                if (!param.isLocked())
                    return false;
            }
        }

        return true;
    }

    private boolean isAnyParameterChanged() {
        for (VoiceGroup g : VoiceGroup.values()) {
            for (BasicParameterModel param : EditBufferModel.get().byVoiceGroup[g.ordinal()].parameters.values()) {
                if (ParameterPresenterProvider.isValueChanged(param))
                    return true;
            }
        }

        return false;
    }

    private void monitorSelectedParameter() {
        EditBufferModel.get().selectedParameter.onChange(selectedParameterID -> {
            int localSubscription = ++selectedParameterSubscription;

            ParameterPresenterProviders.get().registerForCurrentVoiceGroup(selectedParameterID, v -> {
                if (localSubscription != selectedParameterSubscription)
                    return false;

                presenter.selectedParameter = v;
                notifyChanges();
                return true;
            });
            return true;
        });
    }

    private boolean isAnyScaleOffsetParameterNotDefault() {
        for (int scaleParamID : ParameterFactory.getScaleOffsetParameters()) {
            ScaleOffsetParameterModel p = (ScaleOffsetParameterModel) EditBufferModel.get()
                    .getParameter(new ParameterId(scaleParamID, VoiceGroup.Global));

            double currentValue = p.value.getQuantizedAndClipped(true);
            double defaultValue = p.value.metaData.defaultValue.getValue();

            if (currentValue != defaultValue)
                return true;
        }
        return false;
    }

    @Override
    public EditBufferPresenter getValue() {
        return presenter;
    }

    public static EditBufferPresenterProvider get() {
        return theProvider;
    }

    public static EditBufferPresenter getPresenter() {
        return get().presenter;
    }

}