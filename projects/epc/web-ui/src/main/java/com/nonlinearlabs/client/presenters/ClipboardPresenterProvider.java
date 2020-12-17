package com.nonlinearlabs.client.presenters;

import com.nonlinearlabs.client.dataModel.Notifier;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel;
import com.nonlinearlabs.client.dataModel.clipboard.ClipboardModel.DragDataType;

public class ClipboardPresenterProvider extends Notifier<ClipboardPresenter> {
    public static ClipboardPresenterProvider theInstance = new ClipboardPresenterProvider();

    public static ClipboardPresenterProvider get() {
        return theInstance;
    }

    private ClipboardModel model = ClipboardModel.get();
    private ClipboardPresenter presenter = new ClipboardPresenter();

    ClipboardPresenterProvider() {
        model.dnd.onChange(p -> {
            if (p != null)
                presenter.dndType = p.type;
            else
                presenter.dndType = DragDataType.None;
            notifyChanges();
            return true;
        });

    }

    @Override
    public ClipboardPresenter getValue() {
        return presenter;
    }
}