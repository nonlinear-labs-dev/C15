package com.nonlinearlabs.client.world.overlay.undo;

import java.util.ArrayList;
import java.util.function.Function;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.google.gwt.canvas.dom.client.Context2d.TextBaseline;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy;
import com.nonlinearlabs.client.contextStates.ClipContext;
import com.nonlinearlabs.client.world.Control;
import com.nonlinearlabs.client.world.Dimension;
import com.nonlinearlabs.client.world.Gray;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.RGB;
import com.nonlinearlabs.client.world.Rect;
import com.nonlinearlabs.client.world.maps.TransitionDamper;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.OverlayLayout;

public class UndoTree extends OverlayLayout implements TransitionDamper.Client {

	class TransactionIterator {
		UndoTransaction stoppedAtTransaction;

		public void forAllVisibleTransactions(Function<UndoTransaction, Boolean> c) {
			forAllVisibleLayers((Layer layer) -> {
				for (UndoTransaction u : layer.elements) {
					if (!c.apply(u)) {
						stoppedAtTransaction = u;
						return false;
					}
				}
				return true;
			});
		}

		public void forAllVisibleLayers(Function<Layer, Boolean> c) {
			int firstLayer = getFirstVisibleLayer();
			int lastLayer = getLastVisibleLayer();

			int numLayers = layers.getNumLayers();

			firstLayer = Math.min(firstLayer, numLayers - 1);
			firstLayer = Math.max(firstLayer, 0);

			lastLayer = Math.max(lastLayer, 0);
			lastLayer = Math.min(lastLayer, numLayers - 1);

			for (int l = firstLayer; l <= lastLayer; l++) {
				Layer currentLayer = layers.getLayer(l);
				if (!c.apply(currentLayer))
					return;
			}
		}
	}

	private long undoID = 0;
	private long redoID = 0;
	private UndoTransaction root;
	private DragHandle dragHandle;
	private boolean undoPositionUpdated = false;
	private Dimension scrollOffset = new Dimension(0, 0);
	private Layers layers = new Layers();
	private Animator scrollAnimation = null;
	private HorizontalScrollbar horizontalScrollbar;
	private VerticalScrollbar verticalScrollbar;
	private TransitionDamper swishAnimation;

	public UndoTree(UndoTreeWindow world) {
		super(world);
		root = new UndoTransaction(this, null, 0);
		dragHandle = addChild(new DragHandle(this));
		horizontalScrollbar = addChild(new HorizontalScrollbar(this));
		verticalScrollbar = addChild(new VerticalScrollbar(this));
		swishAnimation = new TransitionDamper(0.91, this);
		setupContext(layers.getContext());
	}

	@Override
	public UndoTreeWindow getParent() {
		return (UndoTreeWindow) super.getParent();
	}

	@Override
	public void doLayout(double x, double y, double w, double h) {
		double buttonDim = Millimeter.toPixels(7.5);
		double scrollbarDim = getScrollbarDimension();

		if (w != 0 && h != 0) {
			horizontalScrollbar.doLayout(0, h - scrollbarDim, w - scrollbarDim - buttonDim, scrollbarDim);
			verticalScrollbar.doLayout(w - scrollbarDim, 0, scrollbarDim, h - scrollbarDim - buttonDim);
			dragHandle.doLayout(w - buttonDim, h - buttonDim, buttonDim, buttonDim);
		}

		super.doLayout(x, y, w, h);
	}

	public double getScrollbarDimension() {
		return Millimeter.toPixels(1);
	}

	public void update(Node node) {
		if (node != null) {
			UndoTransaction oldRoot = root;
			updateChildNodes(node);

			if (oldRoot != root) {
				layers.onRootChanged(root);
			}

			if (undoPositionUpdated) {
				handleUndoPositionUpdate();
			}

			sanitizeScrollPosition();
			invalidate(INVALIDATION_FLAG_UI_CHANGED);
		}
	}

	private void handleUndoPositionUpdate() {
		undoPositionUpdated = false;
		UndoTransaction undo = root.findTransaction(undoID);
		if (undo != null) {
			undo.setupCurrentRoute();
		}

		scrollToCurrentUndoStep(true);
	}

	public void scrollToCurrentUndoStep(boolean animate) {
		UndoTransaction undo = root.findTransaction(undoID);

		if (undo == null)
			undo = root;

		if (undo != null) {
			Rect pixRect = undo.calcPixRect(scrollOffset, getPixRect().getLeftTop());

			if (scrollAnimation != null)
				scrollAnimation.cancel();

			scrollAnimation = null;

			double targetY = scrollOffset.getHeight();
			double targetX = scrollOffset.getWidth();

			if (pixRect.getTop() < getPixRect().getTop())
				targetY += getPixRect().getTop() - pixRect.getTop();
			else if (pixRect.getBottom() > getPixRect().getBottom())
				targetY += getPixRect().getBottom() - pixRect.getBottom();

			if (pixRect.getLeft() < getPixRect().getLeft() || pixRect.getRight() > getPixRect().getRight()) {
				double undoTransactionsCenter = pixRect.getCenterPoint().getX();
				double windowsCenter = getPixRect().getCenterPoint().getX();
				double transactionDiffFromCenter = undoTransactionsCenter - windowsCenter;
				targetX -= transactionDiffFromCenter;
			}

			if (targetX != scrollOffset.getWidth() || targetY != scrollOffset.getHeight()) {
				if (animate) {
					scrollAnimation = new Animator(500);
					final Animator thisAnimator = scrollAnimation;

					scrollAnimation.addSubAnimation(scrollOffset.getHeight(), targetY, new Client() {

						@Override
						public void animate(double v) {
							if (thisAnimator == scrollAnimation) {
								scrollOffset.setHeight(v);
								invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
							}
						}
					});

					scrollAnimation.addSubAnimation(scrollOffset.getWidth(), targetX, new Client() {

						@Override
						public void animate(double v) {
							if (thisAnimator == scrollAnimation) {
								scrollOffset.setWidth(v);
								invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
							}
						}
					});
				} else {
					scrollOffset.setHeight(targetY);
					scrollOffset.setWidth(targetX);
					invalidate(INVALIDATION_FLAG_ANIMATION_PROGRESS);
				}
			}
		}
	}

	private void updateChildNodes(Node node) {
		ArrayList<Node> waitList = childNodesToWaitList(node);
		ArrayList<Node> newWaitList = new ArrayList<Node>();

		for (;;) {
			updateProcessWaitList(waitList, newWaitList);

			if (newWaitList.size() == waitList.size())
				break;

			waitList = newWaitList;
			newWaitList = new ArrayList<Node>();
		}
	}

	private ArrayList<Node> childNodesToWaitList(Node node) {
		ArrayList<Node> ret = new ArrayList<Node>();

		NodeList children = node.getChildNodes();
		int numChildren = children.getLength();

		for (int i = 0; i < numChildren; i++)
			ret.add(children.item(i));

		return ret;
	}

	public void updateProcessWaitList(ArrayList<Node> waitList, ArrayList<Node> newWaitList) {
		for (Node child : waitList) {
			if (child.getNodeName().equals("undo")) {
				updateUndo(child);
			} else if (child.getNodeName().equals("redo")) {
				updateRedo(child);
			} else if (child.getNodeName().equals("transaction")) {
				updateTransaction(child, newWaitList);
			}
		}
	}

	private void updateTransaction(Node node, ArrayList<Node> newWaitList) {
		String idStr = node.getAttributes().getNamedItem("id").getNodeValue();
		String predecessorStr = node.getAttributes().getNamedItem("predecessor").getNodeValue();

		long id = Long.decode(idStr);
		long predecessorId = Long.decode(predecessorStr);

		if (predecessorId == 0) {
			updateRoot(node, id);
		} else {
			UndoTransaction transaction = root.findTransaction(id);

			if (transaction != null) {
				transaction.update(id, node);
			} else {
				addNewTransaction(node, id, predecessorId, newWaitList);
			}
		}
	}

	public void addNewTransaction(Node node, long id, long predecessorId, ArrayList<Node> newWaitList) {
		UndoTransaction predecessor = root.findTransaction(predecessorId);

		if (predecessor != null) {
			predecessor.addSuccessor(this, id, node);
		} else {
			newWaitList.add(node);
		}
	}

	public void updateRoot(Node node, long id) {
		UndoTransaction newRoot = root.findTransaction(id);
		if (newRoot != root) {
			if (newRoot != null) {
				root.delete(newRoot);
				root = newRoot;
				root.resetPredecessor();
			}
		}
		root.update(id, node);
	}

	private void updateUndo(Node node) {
		String idStr = ServerProxy.getText(node);
		if (idStr != null) {
			long id = Long.decode(idStr);
			if (id != undoID) {
				undoID = id;
				undoPositionUpdated = true;
				requestLayout();
			}
		}
	}

	private void updateRedo(Node node) {
		String idStr = ServerProxy.getText(node);
		if (idStr != null) {
			long id = Long.decode(idStr);
			if (id != redoID) {
				redoID = id;
				requestLayout();
			}
		}
	}

	public String getCurrentUndoName() {
		return getTransactionName(undoID);
	}

	public String getCurrentRedoName() {
		return getTransactionName(redoID);
	}

	private String getTransactionName(long id) {
		UndoTransaction a = root.findTransaction(id);
		if (a != null)
			return a.getTitle();

		return "";
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ClipContext c = new ClipContext(ctx, this)) {
			getPixRect().fill(ctx, new Gray(26));
			super.draw(ctx, invalidationMask);

			setupContext(ctx);

			try (ClipContext paneClip = new ClipContext(ctx, getPaneRect())) {
				drawTransactions(ctx);
			}
		}

		drawBorder(ctx);
	}

	public void drawBorder(Context2d ctx) {
		getPixRect().stroke(ctx, 2, new RGB(173, 182, 216));
		Rect black = getPixRect().getReducedBy(1);
		double bottom = black.getBottom();
		black.setBottom(bottom);
		black.stroke(ctx, 1, new Gray(0));
	}

	public void setupContext(Context2d ctx) {
		ctx.setTextAlign(TextAlign.CENTER);
		ctx.setFillStyle(getColorFont().toString());
		ctx.setTextBaseline(TextBaseline.MIDDLE);
		ctx.setFont(getFontHeightInPixels() + "px 'SSP-LW25'");
	}

	public void drawTransactions(Context2d ctx) {
		TransactionIterator iterator = new TransactionIterator();
		iterator.forAllVisibleTransactions((UndoTransaction transaction) -> {
			drawUndoTransaction(ctx, transaction);
			return true;
		});
	}

	public void drawUndoTransaction(Context2d ctx, UndoTransaction u) {
		String title = u.getTitle();
		Rect rect = u.calcPixRect(scrollOffset, getPixRect().getLeftTop());

		if (rect != null) {
			if ((u.getID() == undoID) || (undoID == 0 && u == root))
				ctx.setFillStyle(getSelectedColor().toString());
			else
				ctx.setFillStyle(getColorFont().toString());

			Position center = rect.getCenterPoint();
			ctx.fillText(title, center.getX(), center.getY());
			drawSpline(ctx, u);
		}
	}

	public Rect getPaneRect() {
		Rect paneRect = getPixRect().copy();
		paneRect.applyPadding(1, 1, getScrollbarDimension() + 1, getScrollbarDimension() + 1);
		return paneRect;
	}

	public void drawSpline(Context2d ctx, UndoTransaction u) {
		double splineHeight = UndoTransaction.getEntryHeight() - getFontHeightInPixels();
		Rect rect = u.calcPixRect(scrollOffset, getPixRect().getLeftTop());

		if (u.isOnCurrentRoute())
			ctx.setStrokeStyle(getSelectedColor().toString());
		else
			ctx.setStrokeStyle(new Gray(204).toString());

		UndoTransaction predecessor = u.getPredecessor();

		if (predecessor != null) {
			if (predecessor.getSuccessors().size() > 0) {
				Rect predecessorsRect = predecessor.calcPixRect(scrollOffset, getPixRect().getLeftTop());

				if (predecessorsRect != null) {
					drawSpline(ctx,
							new Position(predecessorsRect.getCenterPoint().getX(),
									predecessorsRect.getBottom() - splineHeight / 2),
							new Position(rect.getCenterPoint().getX(), rect.getTop() + splineHeight / 2));
				}
			}
		}
	}

	public RGB getSelectedColor() {
		return new RGB(118, 136, 207);
	}

	public double getFontHeightInPixels() {
		return Millimeter.toPixels(4);
	}

	private void drawSpline(Context2d ctx, Position fromPoint, Position toPoint) {

		ctx.setLineWidth(1);
		ctx.beginPath();

		double dist = toPoint.getX() - fromPoint.getX();

		double radius = Math.min(Math.abs(dist) / 2, 10);
		radius = Math.min(radius, getPixRect().getHeight() / 2);
		double shortenBy = radius;

		if (dist < 0)
			shortenBy = -shortenBy;

		double yCenter = (toPoint.getY() + fromPoint.getY()) / 2;
		ctx.moveTo(fromPoint.getX(), fromPoint.getY());
		ctx.quadraticCurveTo(fromPoint.getX(), yCenter, fromPoint.getX() + shortenBy, yCenter);
		ctx.lineTo(toPoint.getX() - shortenBy, yCenter);
		ctx.quadraticCurveTo(toPoint.getX(), yCenter, toPoint.getX(), toPoint.getY());

		ctx.stroke();
	}

	public long getCurrentUndoID() {
		return undoID;
	}

	public long getCurrentRedoID() {
		return redoID;
	}

	public UndoTransaction getCurrentUndo() {
		return root.getTransaction(undoID);
	}

	public void onDragHandleMoved(double x, double y) {
		getParent().onDragHandleMoved(x, y);
		sanitizeScrollPosition();
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {
		Dimension dim = newPoint.getVector(oldPoint);
		scrollOffset.add(dim);
		swishAnimation.activate(dim.getWidth(), dim.getHeight());
		sanitizeScrollPosition();
		invalidate(INVALIDATION_FLAG_SCROLLED);
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		swishAnimation.run();
		return null;
	}

	public void sanitizeScrollPosition() {
		Rect paneRect = getPaneRect();

		double minScrollY = (-layers.getNumLayers() * UndoTransaction.getEntryHeight()) + getPixRect().getHeight();
		double maxScrollY = 0;

		scrollOffset.setHeight(Math.min(scrollOffset.getHeight(), maxScrollY));
		scrollOffset.setHeight(Math.max(scrollOffset.getHeight(), minScrollY));

		double contentWidth = layers.getMaxLayerWidth();

		double minScrollX = -(contentWidth - paneRect.getWidth());
		double maxScrollX = 0;

		if (minScrollX < 0) {
			scrollOffset.setWidth(Math.min(scrollOffset.getWidth(), maxScrollX));
			scrollOffset.setWidth(Math.max(scrollOffset.getWidth(), minScrollX));
		} else {
			scrollOffset.setWidth((paneRect.getWidth() - contentWidth) / 2);
		}

	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		scrollOffset.add(0, amount * 10);
		sanitizeScrollPosition();
		invalidate(INVALIDATION_FLAG_UI_CHANGED);
		return this;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		return this;
	}

	@Override
	public Control onContextMenu(Position pos) {
		UndoTransaction a = findTransactionAt(pos);
		if (a != null) {
			Overlay o = NonMaps.get().getNonLinearWorld().getViewport().getOverlay();
			UndoTransactionContextMenu m = new UndoTransactionContextMenu(o, a);
			return o.setContextMenu(pos, m);
		}
		return this;
	}

	@Override
	public Control click(Position pos) {
		UndoTransaction a = findTransactionAt(pos);
		if (a != null) {
			a.jump();
			return this;
		}
		return super.click(pos);
	}

	private UndoTransaction findTransactionAt(Position pos) {
		TransactionIterator iterator = new TransactionIterator();
		iterator.forAllVisibleTransactions((UndoTransaction u) -> {
			Rect p = u.calcPixRect(scrollOffset, getPixRect().getLeftTop());
			if (p.contains(pos)) {
				return false;
			}
			return true;
		});
		return iterator.stoppedAtTransaction;
	}

	@Override
	public Control startDragging(Position pos) {
		return this;
	}

	@Override
	public boolean isVisible() {
		if (isIntendedVisible()) {
			if (getPixRect().getHeight() > 0 && getPixRect().getWidth() > 0) {
				return getParent().isRectVisible(getPixRect());
			}
		}
		return false;
	}

	@Override
	public boolean isRectVisible(Rect r) {
		return isVisible() && getPixRect().intersects(r);
	}

	public UndoTransaction getRoot() {
		return root;
	}

	public double getScrollPositionY() {
		return 0;
	}

	public Layers getLayers() {
		return layers;
	}

	public int getFirstVisibleLayer() {
		return (int) (-scrollOffset.getHeight() / UndoTransaction.getEntryHeight());
	}

	public int getLastVisibleLayer() {
		return 2 + ((int) ((-scrollOffset.getHeight() + getPixRect().getHeight()) / UndoTransaction.getEntryHeight()));
	}

	public Dimension getScrollOffset() {
		return scrollOffset;
	}

	public double getScrollPaneWidth() {
		return layers.getMaxLayerWidth();
	}

	public double getScrollPaneHeight() {
		return UndoTransaction.getEntryHeight() * layers.getNumLayers();
	}

	@Override
	public void moveBy(double x, double y) {
		scrollOffset.add(x, y);
		sanitizeScrollPosition();
		invalidate(INVALIDATION_FLAG_SCROLLED);
	}
}
