package com.nonlinearlabs.client.world.overlay;

import java.util.Arrays;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Set;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.core.client.Scheduler;
import com.google.gwt.core.client.Scheduler.RepeatingCommand;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.contextStates.AlphaContextState;
import com.nonlinearlabs.client.contextStates.ContextState;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;

public class DragProxy extends OverlayControl {

	private Position lastMousePos;

	public Position getMousePosition() {
		return lastMousePos;
	}

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
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		try (ContextState state = new AlphaContextState(ctx, 0.5)) {
			bmp.setCoordinateSpaceWidth((int) (origin.getPixRect().getWidth() + 1));
			bmp.setCoordinateSpaceHeight((int) (origin.getPixRect().getHeight() + 1));
			Context2d bgCtx = bmp.getContext2d();
			bgCtx.save();
			bgCtx.translate(-origin.getPixRect().getLeft(), -origin.getPixRect().getTop());
			origin.forceVisibility(true);
			origin.draw(bgCtx, overlay, invalidationMask);
			origin.forceVisibility(false);

			Rect r = getPixRect();

			if (bmp.getCanvasElement().getWidth() > 0 && bmp.getCanvasElement().getHeight() > 0) // Fixes new bug that
																									// whole groups
																									// wouldnt be drawn
																									// sometimes: Failed
																									// to execute
																									// 'drawImage' on
																									// 'CanvasRenderingContext2D':
																									// The image
																									// argument is a
																									// canvas element
																									// with a width or
																									// height of 0.
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
		lastMousePos = newPoint;
		doAutoScrolling(newPoint);

		double xDiff = newPoint.getX() - oldPoint.getX();
		double yDiff = newPoint.getY() - oldPoint.getY();
		NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().moveDragProxies(xDiff, yDiff);
		getOrigin().beingDragged(xDiff, yDiff);

		if (tryHandlingByMousePosition(newPoint))
			return this;

		if (tryHandlingByDragProxyRects())
			return this;

		setReceiver(null, null);
		return this;
	}

	private boolean tryHandlingByDragProxyRects() {
		Set<Control> receivers = new HashSet<Control>();

		for (DragProxy p : NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getDragProxies()) {
			NonMaps.theMaps.getNonLinearWorld().recurseChildren(p.getPixRect(), new ControlFinder() {
				@Override
				public boolean onWayUpFound(Control ctrl) {
					if (!ctrl.isDraggingControl() && (!(ctrl instanceof DragProxy))) {
						receivers.add(ctrl);
					}
					return false;
				}
			});
		}

		Control[] candidates = receivers.toArray(new Control[0]);
		Arrays.sort(candidates, new Comparator<Control>() {

			@Override
			public int compare(Control o1, Control o2) {
				return o2.getDepth() - o1.getDepth();
			}
		});

		for (Control c : candidates) {
			for (DragProxy p : NonMaps.get().getNonLinearWorld().getViewport().getOverlay().getDragProxies()) {
				if (c.getPixRect().intersects(p.getPixRect())) {
					Control r = c.drag(p.getPixRect(), p);
					if (r != null) {
						setReceiver(r, p);
						return true;
					}
				}
			}
		}

		return false;
	}

	public boolean tryHandlingByMousePosition(final Position newPoint) {
		Control p = NonMaps.theMaps.getNonLinearWorld().recurseChildren(newPoint, new ControlFinder() {
			@Override
			public boolean onWayUpFound(Control ctrl) {
				boolean isDragProxy = ctrl instanceof DragProxy;

				if (ctrl != getOrigin() && !isDragProxy) {
					Control r = ctrl.drag(newPoint, DragProxy.this);
					if (r != null) {
						setReceiver(r, DragProxy.this);
						return true;
					}
				}
				return false;
			}
		});

		return p != null;
	}

	public void doAutoScrolling(final Position newPoint) {
		AutoScrollDirection desiredAutoScrollDirection = getDesiredAutoScrollDirection(newPoint);

		if (desiredAutoScrollDirection != currentAutoScrollDirection) {
			setAutoScrolling(desiredAutoScrollDirection);
		}
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
		Rect r = NonMaps.theMaps.getNonLinearWorld().getViewport().getPixRect();
		Rect beltRect = NonMaps.theMaps.getNonLinearWorld().getViewport().getOverlay().getBelt().getPixRect();

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
