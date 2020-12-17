package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;

public class FadeEditorPresenterProvider extends Notifier<FadeEditorPresenter> {
    private static FadeEditorPresenterProvider theProvider = new FadeEditorPresenterProvider();
    private FadeEditorPresenter presenter = new FadeEditorPresenter();

    public static FadeEditorPresenterProvider get() {
        return theProvider;
    }

    @Override
    public FadeEditorPresenter getValue() {
        return presenter;
    }

    public FadeEditorPresenterProvider() {
        for (VoiceGroup vg : new VoiceGroup[] { VoiceGroup.I, VoiceGroup.II }) {

            EditBufferModel.get().getParameter(new ParameterId(396, vg)).onChange(p -> {
                presenter.lastSelectedPart = vg;
                update();
                return true;
            });

            EditBufferModel.get().getParameter(new ParameterId(397, vg)).onChange(p -> {
                presenter.lastSelectedPart = vg;
                update();
                return true;
            });

            EditBufferModel.get().getParameter(new ParameterId(356, vg)).onChange(p -> {
                presenter.lastSelectedPart = vg;
                update();
                return true;
            });
        }
    }

    private void update() {
        EditBufferModel model = EditBufferModel.get();

        BasicParameterModel splitI = model.getParameter(new ParameterId(356, VoiceGroup.I));
        BasicParameterModel splitII = model.getParameter(new ParameterId(356, VoiceGroup.II));

        presenter.splitI.from = 0;
        presenter.splitI.to = (int) Math.round(60 * splitI.value.getQuantizedAndClipped(true));
        presenter.splitI.indicator = presenter.splitI.to;

        presenter.splitII.from = (int) Math.round(60 * splitII.value.getQuantizedAndClipped(true));
        presenter.splitII.to = 60;
        presenter.splitII.indicator = presenter.splitII.from;

        BasicParameterModel fadeI = model.getParameter(new ParameterId(396, VoiceGroup.I));
        BasicParameterModel fadeII = model.getParameter(new ParameterId(396, VoiceGroup.II));
        double fadeIVal = fadeI.value.getQuantizedAndClipped(true);
        double fadeIIVal = fadeII.value.getQuantizedAndClipped(true);

        presenter.fadePointI.from = 0; // 0 means space before key #1
        presenter.fadePointI.to = (int) Math.round(60 * fadeIVal) + 1;
        presenter.fadePointI.indicator = presenter.fadePointI.to;

        presenter.fadePointII.from = (int) Math.round(60 * fadeIIVal);
        presenter.fadePointII.to = 61; // 61 means space between key #61 and #62
        presenter.fadePointII.indicator = presenter.fadePointII.from;

        BasicParameterModel rangeI = model.getParameter(new ParameterId(397, VoiceGroup.I));
        BasicParameterModel rangeII = model.getParameter(new ParameterId(397, VoiceGroup.II));
        double rangeIVal = rangeI.value.getQuantizedAndClipped(true);
        double rangeIIVal = rangeII.value.getQuantizedAndClipped(true);

        double rI = presenter.fadePointI.to + (rangeIVal * 60);
        double rII = presenter.fadePointII.from - (rangeIIVal * 60);

        presenter.fadeRangeI = rI;
        presenter.fadeRangeII = rII;

        presenter.fadePointTextI = fadeI.value.getDecoratedValue(true, true);
        presenter.fadePointTextII = fadeII.value.getDecoratedValue(true, true);

        presenter.fadeRangeTextI = rangeI.value.getDecoratedValue(true, true);
        presenter.fadeRangeTextII = rangeII.value.getDecoratedValue(true, true);

        presenter.splitPointTextI = splitI.value.getDecoratedValue(true, true);
        presenter.splitPointTextII = splitII.value.getDecoratedValue(true, true);

        notifyChanges();
    }
}
