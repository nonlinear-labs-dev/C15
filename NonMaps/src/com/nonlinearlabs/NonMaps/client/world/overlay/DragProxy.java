package com.nonlinearlabs.NonMaps.client.world.overlay;

import java.util.ArrayList;
import java.util.Comparator;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.RepeatingCommand;
import com.nonlinearlabs.NonMaps.client.Animator;
import com.nonlinearlabs.NonMaps.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.NonMaps.client.Millimeter;
import com.nonlinearlabs.NonMaps.client.NonMaps;
import com.nonlinearlabs.NonMaps.client.contextStates.AlphaContextState;
import com.nonlinearlabs.NonMaps.client.contextStates.ContextState;
import com.nonlinearlabs.NonMaps.client.world.Control;
import com.nonlinearlabs.NonMaps.client.world.Position;
import com.nonlinearlabs.NonMaps.client.world.Rect;

public class DragProxy extends OverlayControl {

	class FoundControl {
		public FoundControl(DragProxy proxy, Control ctrl, int ranking) {
			this.proxy = proxy;
			this.ctrl = ctrl;
			this.ranking = ranking;
		}

		Control ctrl = null;
		int ranking = 0;
		DragProxy proxy = null;
	};

	private final Control origin;
	private Control receiver = null;
	private Canvas bmp = Canvas.createIfSupported();
	private Animator animator;
	private Position animated;

	enum AutoScrollDirection {
		None, Up, Down, Left, Right
	}

	RepeatingCommand autoScrollCommand;

	AutoScrollDirection currentAutoScrollDirection = AutoScrollDirection.None;
	private DragProxy triggeredProxy;

	public DragProxy(Overlay parent, Control origin) {
		super(parent);
		this.origin = origin;

		getRelativePosition().set(origin.getPixRect());
	}

	@Override
	public Overlay getParent() {
		return (Overlay) super.getParent();
	}

	public Control getOrigin() {
		return origin;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ContextState state = new AlphaContextState(ctx, 0.5)) {
			bmp.setCoordinateSpaceWidth((int) (origin.getPixRect().getWidth() + 1));
			bmp.setCoordinateSpaceHeight((int) (origin.getPixRect().getHeight() + 1));
			Context2d bgCtx = bmp.getContext2d();
			bgCtx.save();
			bgCtx.translate(-origin.getPixRect().getLeft(), -origin.getPixRect().getTop());
			origin.forceVisibility(true);
			origin.draw(bgCtx, invalidationMask);
			origin.forceVisibility(false);

			Rect r = getPixRect();
			ctx.drawImage(bmp.getCanvasElement(), r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
		}
	}

	@Override
	public void onMouseCaptured() {
		origin.onMouseCaptured();
	}

	@Override
	public void onMouseLost() {
		origin.onMouseLost();
		setAutoScrolling(AutoScrollDirection.None);
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		Rect r = getRelativePosition();
		r.setDimension(origin.getPixRect().getDimension());
		setRelativePosition(r);
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control mouseDrag(Position oldPoint, final Position newPoint, boolean fine) {

		AutoScrollDirection desiredAutoScrollDirection = getDesiredAutoScrollDirection(newPoint);

		if (desiredAutoScrollDirection != currentAutoScrollDirection) {
			setAutoScrolling(desiredAutoScrollDirection);
		}

		double xDiff = newPoint.getX() - oldPoint.getX();
		double yDiff = newPoint.getY() - oldPoint.getY();
		NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().moveDragProxies(xDiff, yDiff);

		getOrigin().beingDragged(xDiff, yDiff);

		ArrayList<FoundControl> foundControls = new ArrayList<FoundControl>();

		for (DragProxy p : NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getDragProxies()) {
			NonMaps.theMaps.getNonLinearWorld().recurseChildren(p.getPixRect(), new ControlFinder() {
				@Override
				public boolean onWayUpFound(Control ctrl) {
					boolean isDragProxy = ctrl instanceof DragProxy;

					if (ctrl != getOrigin() && !isDragProxy) {

						for (FoundControl a : foundControls) {
							if (a.ctrl == ctrl) {
								return false;
							}
						}

						int ranking = ctrl.getDragRating(newPoint, p);
						if (ranking >= 0) {
							foundControls.add(new FoundControl(p, ctrl, ranking));
						}
					} else if(ctrl == getOrigin()) {
						if(ctrl.stopDragCompletelyIfDraggedOn()) {
							foundControls.clear();
							return true;
						}
					}
					return false;
				}
			});
		}

		foundControls.sort(new Comparator<FoundControl>() {

			@Override
			public int compare(FoundControl o1, FoundControl o2) {
				return o2.ranking - o1.ranking;
			}
		});

		for (FoundControl f : foundControls) {
			Control r = f.ctrl.drag(newPoint, f.proxy);
			if (r != null) {
				setReceiver(r, f.proxy);
				return this;
			} else {
			}
		}

		setReceiver(null, null);
		return this;
	}

	public void moveProxy(double xDiff, double yDiff) {
		getRelativePosition().moveBy(xDiff, yDiff);
		calcPixRect(getParent().getPixRect().getPosition(), 1);
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
	}

	private void setAutoScrolling(AutoScrollDirection desiredAutoScrollDirection) {
		currentAutoScrollDirection = desiredAutoScrollDirection;

		if (currentAutoScrollDirection == AutoScrollDirection.None) {
			autoScrollCommand = null;
		} else {
			autoScrollCommand = new RepeatingCommand() {

				@Override
				public boolean execute() {
					if (autoScrollCommand == this) {
						if (currentAutoScrollDirection != AutoScrollDirection.None) {
							switch (currentAutoScrollDirection) {
							case Down:
								NonMaps.theMaps.getNonLinearWorld().getViewport().autoScroll(0, 1);
								break;
							case Left:
								NonMaps.theMaps.getNonLinearWorld().getViewport().autoScroll(-1, 0);
								break;
							case Right:
								NonMaps.theMaps.getNonLinearWorld().getViewport().autoScroll(1, 0);
								break;
							case Up:
								NonMaps.theMaps.getNonLinearWorld().getViewport().autoScroll(0, -1);
								break;
							default:
								break;
							}
							return true;
						}
					}
					return false;
				}
			};

			Scheduler.get().scheduleFixedPeriod(autoScrollCommand, 50);
		}
	}

	private AutoScrollDirection getDesiredAutoScrollDirection(Position newPoint) {
		Rect r = NonMaps.theMaps.getNonLinearWorld().getViewport().getPixRectWithoutBelt();
		Rect beltRect = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getBelt().getPixRect();

		if (beltRect.contains(newPoint))
			return AutoScrollDirection.None;

		double borderSize = Millimeter.toPixels(10);

		if (newPoint.getX() < r.getLeft() + borderSize)
			return AutoScrollDirection.Left;

		if (newPoint.getX() > r.getRight() - borderSize)
			return AutoScrollDirection.Right;

		if (newPoint.getY() < r.getTop() + borderSize)
			return AutoScrollDirection.Up;

		if (newPoint.getY() > r.getBottom() - borderSize)
			return AutoScrollDirection.Down;

		return AutoScrollDirection.None;
	}

	@Override
	public Control mouseUp(final Position eventPoint) {
		drop(eventPoint);
		if (NonMaps.get().getNonLinearWorld().getViewport().getOverlay().removeDragProxies())
			invalidate(INVALIDATION_FLAG_VISIBILITY_CHANGED);
		return this;
	}

	public void drop(final Position newPoint) {
		if (receiver != null) {
			Control r = receiver.drop(newPoint, triggeredProxy);

			if (r != null) {
				setReceiver(r, triggeredProxy);
			}
		}

		if (triggeredProxy != null) {
			triggeredProxy.originBeingDropped();
		}
	}

	private void originBeingDropped() {
		if (origin != null)
			origin.beingDropped();

	}

	protected void setReceiver(Control r, DragProxy proxy) {
		if (receiver != null && receiver != r) {
			receiver.dragLeave();
		}
		receiver = r;
		this.triggeredProxy = proxy;
	}

	@Override
	public boolean wantsMouseUpAnimation() {
		return false;
	}

	public void animatePositionOffset(double d, double e) {
		animator = new Animator(1000);
		animated = new Position(0, 0);
		animator.addSubAnimation(0, d, new Client() {

			@Override
			public void animate(double v) {
				double now = v - animated.getX();
				animated.setX(v);
				getRelativePosition().moveBy(now, 0);
			}
		});

		animator.addSubAnimation(0, e, new Client() {

			@Override
			public void animate(double v) {
				double now = v - animated.getY();
				animated.setY(v);
				getRelativePosition().moveBy(0, now);
				requestLayout();
			}
		});
	}

	public Control getCurrentReceiver() {
		return receiver;
	}
}
