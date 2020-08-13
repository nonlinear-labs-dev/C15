package com.nonlinearlabs.client.world.maps.presets.bank;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.GWT;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;

public class Tape extends MapsControl {

	public enum Orientation {
		North, South, East, West
	}

	public Orientation invertOrientation(Orientation o) {
		switch (o) {
			case East:
				return Orientation.West;
			case North:
				return Orientation.South;
			case South:
				return Orientation.North;
			case West:
				return Orientation.East;
		}
		return null;
	}

	private Orientation orientation;
	private Bank prospectBank;

	public Tape(Bank parent, Orientation orientation) {
		super(parent);
		this.setOrientation(orientation);
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
	}

	@Override
	public Bank getParent() {
		return (Bank) super.getParent();
	}

	@Override
	public boolean isVisible() {
		return (isActiveEmptyTape() || isActiveInsertTape());
	}

	private boolean isActiveEmptyTape() {
		Overlay o = getNonMaps().getNonLinearWorld().getViewport().getOverlay();

		for (DragProxy d : o.getDragProxies()) {
			Control r = d.getCurrentReceiver();
			if (r != null) {
				if (r instanceof PresetManager || r instanceof Tape) {
					return super.isVisible() && o.isCurrentlyDraggingATypeOf(Bank.class.getName())
							&& getParent().isTapeActive(orientation);
				}
			}
		}

		return false;
	}

	private DragProxy getDragProxyOf(Tape t) {
		for (DragProxy d : NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getDragProxies()) {
			if (d.getOrigin() == t.getParent()) {
				return d;
			}
		}
		return null;
	}

	private Tape getTopMostIntersectingTape(Tape draggedTape) {

		if (isOrientedHorizontal() || draggedTape.isOrientedHorizontal())
			return this;

		DragProxy dragProxy = getDragProxyOf(draggedTape);

		if (dragProxy == null)
			return null;

		Bank draggedBank = draggedTape.getParent();
		Dimension offset = dragProxy.getPixRect().getLeftTop().getVector(draggedBank.getPixRect().getLeftTop());
		Rect draggedRect = draggedBank.header.getPixRect().getMovedBy(offset);

		double yDiff = Double.MAX_VALUE;
		Tape curr = null;

		for (Bank bank : this.getParent().getPresetManager().getBanks()) {
			if (bank == draggedBank)
				continue;

			for (Tape tape : bank.getTapes()) {
				if (tape == draggedTape)
					continue;
				if (!tape.getPixRect().intersects(draggedRect))
					continue;

				double diff = Math.abs(draggedRect.getTop() - tape.getParent().getPixRect().getTop());
				if (diff < yDiff) {
					yDiff = diff;
					curr = tape;
				}
			}
		}

		return curr;
	}

	public boolean isOrientedHorizontal() {
		return orientation == Orientation.North || orientation == Orientation.South;
	}

	private boolean isInsertTape() {
		return getParent().isDockedInDirection(getOrientation());
	}

	public boolean isCurrentlyDraggedOverMe() {
		if (!isInsertTape())
			return false;

		Tape otherTape = null;
		if (getParent().getMaster() != null) {
			otherTape = getParent().getMaster().getTape(invertOrientation(getOrientation()));
		}

		Overlay o = getNonMaps().getNonLinearWorld().getViewport().getOverlay();
		for (DragProxy d : o.getDragProxies()) {
			Control r = d.getCurrentReceiver();
			if (r == this || (otherTape != null && r == otherTape))
				if ((getPixRect().contains(d.getMousePosition())
						|| otherTape.getPixRect().contains(d.getMousePosition())) && prospectBank != null)
					return true;
		}
		return false;
	}

	public void layout(NonDimension oldDim) {

		double tapeSize = getParent().getAttachArea();

		switch (getOrientation()) {

			case North:
				setNonSize(new NonDimension(oldDim.getWidth(), tapeSize));
				moveTo(new NonPosition(tapeSize, 0));
				break;
			case South:
				setNonSize(new NonDimension(oldDim.getWidth(), tapeSize));
				moveTo(new NonPosition(tapeSize, oldDim.getHeight() + tapeSize));
				break;
			case East:
				setNonSize(new NonDimension(tapeSize, oldDim.getHeight()));
				moveTo(new NonPosition(oldDim.getWidth() + tapeSize, 0 + tapeSize));
				break;
			case West:
				setNonSize(new NonDimension(tapeSize, oldDim.getHeight()));
				moveTo(new NonPosition(0, tapeSize));
				break;
		}
	}

	private boolean isActiveInsertTape() {
		Overlay o = getNonMaps().getNonLinearWorld().getViewport().getOverlay();

		for (DragProxy d : o.getDragProxies()) {
			Control r = d.getCurrentReceiver();
			if (r != null) {

				if ((r instanceof PresetManager || r instanceof Tape) && d.getOrigin() instanceof Bank) {
					Bank b = (Bank) d.getOrigin();

					boolean visible = super.isVisible();
					boolean bankIsNotInCluster = !b.isInCluster();

					return visible && o.isCurrentlyDraggingATypeOf(Bank.class.getName())
							&& getParent().isDockedInDirection(getOrientation()) && bankIsNotInCluster;
				}
			}
		}

		return false;
	}

	private Orientation getOrientation() {
		return orientation;
	}

	private void setOrientation(Orientation orientation) {
		this.orientation = orientation;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		super.draw(ctx, invalidationMask);
		Rect r = getPixRect().copy();
		prepareRectForDraw(r);
		r.fill(ctx, isInsertTape() ? getInsertColor() : getTapeColor());
	}

	private void prepareRectForDraw(Rect r) {
		switch (orientation) {
			case East:
				r.setWidth(r.getWidth() / 2);
				break;

			case North:
				r.setHeight(r.getHeight() / 2);
				r.setTop(r.getTop() + r.getHeight());
				break;

			case South:
				r.setHeight(r.getHeight() / 2);
				break;

			case West:
				r.setWidth(r.getWidth() / 2);
				r.setLeft(r.getLeft() + r.getWidth());
				break;

			default:
				break;

		}
	}

	private Tape findOppositeInsertTape() {
		return getParent().getAttachedTapeInDirection(getOrientation());
	}

	private RGB getInsertColor() {
		RGB activeColor = new RGB(172, 185, 198);
		Tape opposite = findOppositeInsertTape();
		boolean otherActive = false;

		if (opposite != null)
			otherActive = opposite.isCurrentlyDraggedOverMe();
		else
			GWT.log("should not happen! " + toString());

		if (isCurrentlyDraggedOverMe() || otherActive)
			return activeColor;

		return new RGB(51, 83, 171);
	}

	private RGB getTapeColor() {
		RGB activeColor = new RGB(172, 185, 198);

		return getParent().getPresetManager().isAttachingTape(this) ? activeColor : new RGB(51, 83, 171);
	}

	private boolean fitsTo(Tape others) {
		if (getParent().isClusteredWith(others.getParent()))
			return false;

		if (!isVisible())
			return false;

		if (!others.isVisible())
			return false;

		if (!isInvertedOrientation(others.orientation))
			return false;

		if (getTopMostIntersectingTape(others) != this)
			return false;

		return true;
	}

	private boolean isInvertedOrientation(Orientation other) {
		return (orientation == Orientation.East && other == Orientation.West)
				|| (orientation == Orientation.West && other == Orientation.East)
				|| (orientation == Orientation.North && other == Orientation.South)
				|| (orientation == Orientation.South && other == Orientation.North);
	}

	private boolean fitsTo(Bank other) {
		if (getParent().isClusteredWith(other))
			return false;

		if (!isVisible())
			return false;

		return other.isVisible();
	}

	@Override
	public Control drag(Rect pos, DragProxy dragProxy) {
		if (isInsertTape())
			return insertTapeDrag(pos, dragProxy);

		if (isVisible() && !getParent().isDraggingControl()) {
			if (dragProxy.getOrigin() instanceof Bank) {
				Bank other = (Bank) dragProxy.getOrigin();

				if (getParent() != other) {
					Dimension offset = dragProxy.getPixRect().getLeftTop().getVector(other.getPixRect().getLeftTop());

					for (Tape others : other.getTapes()) {
						if (fitsTo(others)) {
							if (getPixRect().intersects(others.getPixRect().getMovedBy(offset))) {
								getParent().getPresetManager().setAttachingTapes(this, others);
								return this;
							}
						}
					}
				}
			}
		}
		return super.drag(pos, dragProxy);
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (isActiveInsertTape())
			return insertTapeDrop(pos, dragProxy);

		if (dragProxy.getOrigin() instanceof Bank) {
			Bank other = (Bank) dragProxy.getOrigin();
			Bank clusterMaster = other.getClusterMaster();
			DragProxy dragProxyForClusterMaster = NonMaps.get().getNonLinearWorld().getViewport().getOverlay()
					.getDragProxyFor(clusterMaster);
			Position dropPosition = dragProxyForClusterMaster != null
					? dragProxyForClusterMaster.getPixRect().getPosition()
					: pos;
			NonPosition nonPos = NonMaps.get().getNonLinearWorld().toNonPosition(dropPosition);
			nonPos.snapTo(PresetManager.getSnapGridResolution());
			NonMaps.get().getServerProxy().dockBanks(getParent(), orientation, other, nonPos);
			other.getClusterMaster().moveTo(nonPos);

			requestLayout();
			return this;
		}
		return super.drop(pos, dragProxy);
	}

	private void setShouldInsert(Bank b) {
		prospectBank = b;
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	private boolean shouldInsert(Bank b) {
		return b == prospectBank;
	}

	private Control insertTapeDrag(Rect pos, DragProxy dragProxy) {
		if (isVisible() && !getParent().isDraggingControl()) {
			if (dragProxy.getOrigin() instanceof Bank) {
				Bank other = (Bank) dragProxy.getOrigin();

				if (getPixRect().contains(dragProxy.getMousePosition())) {
					if (getParent() != other) {
						if (fitsTo(other)) {
							setShouldInsert(other);
							return this;
						}
					}
				}
				setShouldInsert(null);
			}
		}
		return super.drag(pos, dragProxy);
	}

	private Control insertTapeDrop(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof Bank) {
			Bank other = (Bank) dragProxy.getOrigin();
			if (shouldInsert(other)) {
				NonMaps.get().getServerProxy().insertBankInCluster(other, orientation, this.getParent());
				requestLayout();
			}
			return this;
		}
		return super.drop(pos, dragProxy);
	}
}
