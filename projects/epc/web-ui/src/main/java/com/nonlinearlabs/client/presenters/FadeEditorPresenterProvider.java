package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.editBuffer.BasicParameterModel;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel;
import com.nonlinearlabs.client.dataModel.editBuffer.ParameterId;
import com.nonlinearlabs.client.dataModel.editBuffer.EditBufferModel.VoiceGroup;

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
        for(VoiceGroup vg: new VoiceGroup[]{VoiceGroup.I, VoiceGroup.II}) {
            
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
        presenter.splitI.to = Math.max(1, Math.min((int)(61 * splitI.value.value.getValue()) + 1, 61));
        presenter.splitI.indicator = presenter.splitI.to;

        presenter.splitII.from = Math.max(0, Math.min((int)(61 * splitII.value.value.getValue()), 60));
        presenter.splitII.to = 61;
        presenter.splitII.indicator = presenter.splitII.from;


        BasicParameterModel fadeI = model.getParameter(new ParameterId(396, VoiceGroup.I));
        BasicParameterModel fadeII = model.getParameter(new ParameterId(396, VoiceGroup.II));

        presenter.fadePointI.from = 0;
        presenter.fadePointI.to = Math.max(1, Math.min((int)(61 * fadeI.value.value.getValue()) + 1, 61));
        presenter.fadePointI.indicator = presenter.fadePointI.to;

        presenter.fadePointII.from = Math.max(0, Math.min((int)(61 * fadeII.value.value.getValue()), 60));
        presenter.fadePointII.to = 61;
        presenter.fadePointII.indicator = presenter.fadePointII.from;

        BasicParameterModel rangeI = model.getParameter(new ParameterId(397, VoiceGroup.I));
        BasicParameterModel rangeII = model.getParameter(new ParameterId(397, VoiceGroup.II));

        double rI = Math.max(Math.min(presenter.fadePointI.to + (rangeI.value.value.getValue() * 61), 61), presenter.fadePointI.to);
        double rII = Math.min(Math.max(presenter.fadePointII.from - (rangeII.value.value.getValue() * 61), 0), presenter.fadePointII.from);

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
