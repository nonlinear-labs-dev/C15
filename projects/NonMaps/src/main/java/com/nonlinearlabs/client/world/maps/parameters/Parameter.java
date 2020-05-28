package com.nonlinearlabs.client.world.maps.parameters;

import com.google.gwt.canvas.dom.client.Context2d;
import com.nonlinearlabs.client.Checksum;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.Tracer;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.EditParameter;
import com.nonlinearlabs.client.presenters.ParameterPresenter;
import com.nonlinearlabs.client.presenters.ParameterPresenterProviders;
import com.nonlinearlabs.client.useCases.EditBufferUseCases;
import com.nonlinearlabs.client.useCases.IncrementalChanger;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.LayoutResizingVertical;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.overlay.ContextMenu;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.belt.parameters.ParameterContextMenu;
import com.nonlinearlabs.client.world.pointer.TouchPinch;

public abstract class Parameter extends LayoutResizingVertical {

	public ParameterPresenter presenter;

	protected IncrementalChanger currentParameterChanger = null;
	protected int parameterNumber;

	public Parameter(MapsLayout parent, int number) {
		super(parent);
		this.parameterNumber = number;
		ParameterPresenterProviders.get().registerForCurrentVoiceGroup(number, p -> onPresenterUpdated(p));
	}

	private boolean onPresenterUpdated(ParameterPresenter p) {
		presenter = p;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
		return true;
	}

	@Override
	public void getStateHash(Checksum crc) {
		presenter.getHash(crc);
	}

	private boolean shouldHightlightChanged() {
		return presenter.highlightChanged;
	}

	public int getParameterNumber() {
		return parameterNumber;
	}

	protected RGB getRoundingColor() {
		if (isSelected()) {
			return getColorSliderHighlight();
		} else if (shouldHightlightChanged()) {
			return RGB.changedBorder();
		}
		return null;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {

		getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), 0, getParameterBackgroundColor(),
				null);

		super.draw(ctx, invalidationMask);

		RGB c = getRoundingColor();
		if (c != null) {
			getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), toXPixels(1), null, c);
		}

		if (presenter != null && presenter.disabled) {
			getPixRect().drawRoundedRect(ctx, getBackgroundRoundings(), toXPixels(4), toXPixels(1),
					RGB.black().withAlpha(0.5), RGB.black().withAlpha(0.2));
		}
	}

	private RGB getParameterBackgroundColor() {
		return isSelected() ? getColorObjectBackgroundSelected() : new Gray(52);
	}

	protected int getBackgroundRoundings() {
		int numChildrenInParent = getParent().getChildren().size();

		if (numChildrenInParent == 1)
			return Rect.ROUNDING_ALL;

		if (getParent().getChildren().get(0) == this)
			return Rect.ROUNDING_TOP;

		if (getParent().getChildren().get(numChildrenInParent - 1) == this)
			return Rect.ROUNDING_BOTTOM;

		return Rect.ROUNDING_NONE;
	}

	public void setDefault() {
		EditBufferUseCases.get().setToDefault(getParameterNumber());
	}

	public void select() {
		EditBufferUseCases.get().selectParameter(getParameterNumber());
	}

	public boolean isSelected() {
		return presenter.selected;
	}

	@Override
	public Control click(Position eventPoint) {
		boolean isAnyChildVisible = isAnyChildVisible();

		if (!isAnyChildVisible)
			return null;

		if (!isSelected())
			select();

		return this;
	}

	@Override
	public Control doubleClick(Position pos) {
		if (!presenter.disabled && !presenter.hidden)
			setDefault();
		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {

		switch (SetupModel.get().localSettings.editParameter.getValue()) {
			case always:
				select();

				if (isBoolean() && !presenter.disabled)
					toggleBoolean();
				else if (!presenter.disabled)
					startMouseEdit();
				return this;

			case if_selected:
				if (isSelected()) {
					if (isBoolean() && !presenter.disabled)
						toggleBoolean();
					else if (!presenter.disabled)
						startMouseEdit();
					return this;
				}
				break;

			case never:
				break;

			default:
				break;
		}

		return super.mouseDown(eventPoint);
	}

	protected void startMouseEdit() {
		currentParameterChanger = EditBufferUseCases.get().startEditParameterValue(getParameterNumber(),
				Millimeter.toPixels(100));
	}

	@Override
	public boolean wantsMouseUpAnimation() {
		return false;
	}

	@Override
	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		if (isSelected()) {
			Tracer.log("Parameter.pinch");
			return NonMaps.theMaps.getNonLinearWorld();
		}
		return super.pinch(eventPoint, touchDist, pinch);
	}

	public boolean forceFine(boolean in) {
		return in;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		fine = forceFine(fine);

		boolean noDrag = (SetupModel.get().localSettings.editParameter.getValue() == EditParameter.never)
				|| getWorld().isSpaceDown();

		if (isSelected() && !noDrag && !presenter.disabled && !presenter.hidden) {

			double xPix = newPoint.getX() - oldPoint.getX();
			double yPix = oldPoint.getY() - newPoint.getY();
			double pix = xPix;

			if (Math.abs(yPix) > Math.abs(xPix))
				pix = yPix;

			if (currentParameterChanger != null)
				currentParameterChanger.changeBy(fine, pix);

			return this;
		} else if (noDrag) {
			return getWorld().mouseDrag(oldPoint, newPoint, fine);
		} else if (presenter.disabled || presenter.hidden) {
			return this;
		}

		return null;
	}

	@Override
	public void onMouseLost() {
		if (currentParameterChanger != null) {
			currentParameterChanger.finish();
			currentParameterChanger = null;
		}
		super.onMouseLost();
	}

	private boolean isAnyChildVisible() {
		boolean isAnyChildVisible = false;

		for (MapsControl c : getChildren()) {
			isAnyChildVisible |= c.isVisible();

			if (isAnyChildVisible)
				break;
		}
		return isAnyChildVisible;
	}

	protected ParameterEditor getSelectionRoot() {
		return ParameterEditor.get();
	}

	public void inc(boolean fine) {
		EditBufferUseCases.get().incParameter(getParameterNumber(), fine);
	}

	public void dec(boolean fine) {
		EditBufferUseCases.get().decParameter(getParameterNumber(), fine);
	}

	public String getParameterGroupID() {
		return getParameterGroup().getName();
	}

	public boolean isBoolean() {
		return presenter.isBoolean;
	}

	private void toggleBoolean() {
		EditBufferUseCases.get().toggleBoolean(getParameterNumber());
	}

	public ContextMenu createContextMenu(Overlay o) {
		return new ParameterContextMenu(o, this);
	}

	public boolean dimHandleAtDefaultValue() {
		return false;
	}

	public boolean isOracle() {
		return true;
	}

	public static Parameter findInParents(MapsControl c) {
		if (c instanceof Parameter)
			return (Parameter) c;

		return findInParents(c.getParent());
	}

	public ParameterGroup getParameterGroup() {
		Control p = getParent();
		while (p != null) {
			if (p instanceof ParameterGroup)
				return (ParameterGroup) p;

			p = p.getParent();
		}
		return null;
	}

	@Override
	public Control onContextMenu(Position pos) {
		select();
		boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;

		if (showContextMenus) {
			Overlay o = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay();
			return o.setContextMenu(pos, createContextMenu(o));
		}
		return super.onContextMenu(pos);
	}

	public String getFullNameWithGroup() {
		return presenter.fullNameWithGroup;
	}

	public String getGroupName() {
		return presenter.longName;
	}

	public boolean isLocked() {
		return presenter.locked;
	}

	@Override
	public double getBottomMargin() {
		if (presenter.hidden)
			return 0;
		return super.getBottomMargin();
	}

	@Override
	public double getTopMargin() {
		if (presenter.hidden)
			return 0;
		return super.getTopMargin();
	}

	@Override
	public double getHeightMargin() {
		if (presenter.hidden)
			return 0;
		return super.getHeightMargin();
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		if (presenter.hidden) {
			setNonSize(new NonDimension(0, 0));
		} else {
			super.doFirstLayoutPass(levelOfDetail);
		}
	}

}
