package com.nonlinearlabs.client.world;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.Composite;
import com.google.gwt.canvas.dom.client.Context2d.LineCap;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.user.client.Window;
import com.nonlinearlabs.client.Animator;
import com.nonlinearlabs.client.Animator.DoubleClientData.Client;
import com.nonlinearlabs.client.ClipboardManager;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.contextStates.ContextState;
import com.nonlinearlabs.client.contextStates.StopWatchState;
import com.nonlinearlabs.client.dataModel.setup.SetupModel;
import com.nonlinearlabs.client.dataModel.setup.SetupModel.BooleanValues;
import com.nonlinearlabs.client.integrationTests.IntegrationTests;
import com.nonlinearlabs.client.world.maps.IContextMenu;
import com.nonlinearlabs.client.world.maps.MapsControl;
import com.nonlinearlabs.client.world.maps.MapsLayout;
import com.nonlinearlabs.client.world.maps.NonDimension;
import com.nonlinearlabs.client.world.maps.NonPosition;
import com.nonlinearlabs.client.world.maps.NonRect;
import com.nonlinearlabs.client.world.maps.TransitionDamper;
import com.nonlinearlabs.client.world.maps.parameters.ParameterEditor;
import com.nonlinearlabs.client.world.maps.presets.PresetManager;
import com.nonlinearlabs.client.world.maps.settings.DeveloperSettings;
import com.nonlinearlabs.client.world.overlay.DragProxy;
import com.nonlinearlabs.client.world.overlay.Overlay;
import com.nonlinearlabs.client.world.overlay.belt.presets.PresetManagerContextMenu;
import com.nonlinearlabs.client.world.pointer.TouchPinch;

public class NonLinearWorld extends MapsLayout {

	private Animator theAnimation = null;
	private int invalidationMask = 0;
	private NonMaps root = null;
	private final ParameterEditor parameterEditor;
	private final ClipboardManager clipboardManager;
	private final PresetManager presetManager;
	private final Viewport viewport;
	private final DeveloperSettings settings;
	private boolean layoutRequested = false;
	private boolean firstLayout = true;
	private boolean isShiftDown = false;
	private boolean isSpaceDown = false;
	private boolean isCtrlDown = false;

	private double maxLevelOfDetail = 0;

	private TransitionDamper scrollAnimation = new TransitionDamper(0.9, new TransitionDamper.Client() {

		@Override
		public void moveBy(double x, double y) {
			scroll(x, y);
		}
	});

	public NonLinearWorld(NonMaps root) {
		super(null);
		this.root = root;

		settings = addChild(new DeveloperSettings(this));
		parameterEditor = new ParameterEditor(this);
		presetManager = new PresetManager(this);
		clipboardManager = new ClipboardManager();

		addChild(parameterEditor);
		addChild(presetManager);

		viewport = addChild(new Viewport(this));

		maxLevelOfDetail = getMaxLevelOfDetail();

		getNonPosition().set(new NonRect(-1000, -1000, 2000, 2000));
		presetManager.getNonPosition().set(new NonRect(-1000, -1000, 2000, 2000));
		viewport.getNonPosition().set(new NonRect(-100, -100, 200, 200));
		parameterEditor.getNonPosition().set(new NonRect(-100, -100, 200, 200));
		viewport.setScreenDimension(100, 100);

		layoutRequested = false;

		SetupModel.get().systemSettings.forceHighlightChangedParameters.onChange(t -> {
			invalidate(Control.INVALIDATION_FLAG_UI_CHANGED);
			return true;
		});
	}

	public void init() {
		viewport.initViewport();
	}

	@Override
	public int getDepth() {
		return 0;
	}

	public DeveloperSettings getSettings() {
		return settings;
	}

	@Override
	public NonLinearWorld getWorld() {
		return this;
	}

	public Viewport getViewport() {
		return viewport;
	}

	@Override
	public NonMaps getNonMaps() {
		return root;
	}

	public ParameterEditor getParameterEditor() {
		return parameterEditor;
	}

	public PresetManager getPresetManager() {
		return presetManager;
	}

	public ClipboardManager getClipboardManager() {
		return clipboardManager;
	}

	@Override
	public void invalidate(int flags) {
		invalidationMask |= flags;
	}

	@Override
	public boolean isRectVisible(Rect r) {
		return viewport.getPixRect().intersects(r);
	}

	public int getAndResetInvalid() {

		if (layoutRequested) {
			try (StopWatchState stop = new StopWatchState("NonlinearWorld::getAndResetInvalid -> doLayout")) {
				layoutRequested = false;
				doFirstLayoutPass(getCurrentLevelOfDetail());
				invalidationMask |= INVALIDATION_FLAG_UI_CHANGED;
			}
		}

		int ret = invalidationMask;
		invalidationMask = 0;
		return ret;
	}

	@Override
	public boolean zoomTo(MapsControl c) {
		double oldZoom = getCurrentZoom();

		// find pix rect when zoomed out to detail level that shows all details
		zoomToMaxLevelOfDetail();
		doFirstLayoutPass(getCurrentLevelOfDetail());
		calcPixRects();
		Rect r = c.getPixRect().copy();

		r.applyPadding(-10, -10, -10, -10);

		// convert to non pos
		NonPosition nonPos = c.toNonPosition(r.getPosition());
		NonDimension nonDim = c.toNonDimension(r.getDimension());

		// restore previous zoom level
		getViewport().setCurrentZoom(oldZoom);
		doFirstLayoutPass(getCurrentLevelOfDetail());
		calcPixRects();

		// zoom to non pos in zoomed out detail
		return zoomTo(new NonRect(nonPos, nonDim), true);
	}

	public void setScreenSize(int width, int height) {
		viewport.setScreenDimension(width, height);
		viewport.setNonSize(width, height);
		viewport.moveTo(-width / 2, -height / 2);
		requestLayout();
	}

	@Override
	public void doFirstLayoutPass(double levelOfDetail) {
		double maxDimension = 10000000;
		double width = maxDimension;
		double height = maxDimension;

		for (MapsControl c : getChildren()) {
			c.doFirstLayoutPass(levelOfDetail);
			c.doSecondLayoutPass(width, height);
		}

		setNonSize(width, height);
		getNonPosition().moveTo(-maxDimension / 2, -maxDimension / 2);

		parameterEditor.moveTo(-parameterEditor.getNonPosition().getWidth() / 2,
				-parameterEditor.getNonPosition().getHeight() / 2);

		if (firstLayout) {
			settings.getNonPosition().moveTo(-settings.getNonPosition().getWidth() / 2,
					-2 * parameterEditor.getNonPosition().getHeight());
		}

		calcPixRects();

		firstLayout = false;
	}

	private void calcPixRects() {
		calcPixRect(getViewPortsPixelVector().getNegated(), getCurrentZoom());
	}

	public void calcPixRect(Position parentsReference, double currentZoom) {
		if (calcPixRectWithoutMargins(parentsReference, currentZoom))
			children.calcPixRect(this.getPixRect().getCenterPoint(), currentZoom);
	}

	private Position getViewPortsPixelVector() {
		final double zoom = getCurrentZoom();
		final NonPosition viewPortsPosition = viewport.getNonPosition().getPosition();
		return new Position(viewPortsPosition.getX() * zoom, viewPortsPosition.getY() * zoom);
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		try (ContextState state = new ContextState(ctx)) {
			try (StopWatchState s = new StopWatchState("NonlinearWorld::draw")) {
				ctx.save();
				ctx.setGlobalCompositeOperation(Composite.SOURCE_OVER);
				ctx.setLineCap(LineCap.BUTT);
				ctx.translate(-0.5, -0.5);
				viewport.drawBackground(ctx);
				drawChildren(ctx, invalidationMask);
				viewport.draw(ctx, invalidationMask);

				if (getPresetManager().hasMultipleRectangle())
					getPresetManager().getMoveSomeBanks().draw(ctx, invalidationMask);

				ctx.restore();
			}
		}
	}

	@Override
	public RGB getColorFont() {
		return new RGB(210, 210, 210);
	}

	@Override
	public RGB getColorFontValueDisplay() {
		return new RGB(250, 0, 0);
	}

	@Override
	public RGB getColorFontValueDisplaySelected() {
		return new RGB(0, 255, 0);
	}

	@Override
	public RGB getColorSliderHighlight() {
		return new RGB(120, 0, 0);
	}

	@Override
	public RGB getColorSliderBackground() {
		return new RGB(0, 120, 0);
	}

	@Override
	public RGB getColorModuleBackground() {
		return new RGB(0, 0, 120);
	}

	@Override
	public RGB getColorModuleHeaderBackground() {
		return new RGB(120, 120, 0);
	}

	@Override
	public RGB getHeaderColorFont() {
		return new RGB(30, 30, 30);
	}

	@Override
	public double getLevelOfDetailForFullVisibility() {
		return 1;
	}

	@Override
	public Control mouseDown(Position eventPoint) {
		if (scrollAnimation != null)
			scrollAnimation.stop();

		if (removeContextMenus())
			requestLayout();

		return this;
	}

	@Override
	public Control mouseDrag(Position oldPoint, Position newPoint, boolean fine) {

		if (isShiftDown()) {
			if (getPresetManager().hasMultipleRectangle()) {
				getPresetManager().updateMultipleRectangle(newPoint);
			} else {
				getPresetManager().startMultipleRectangle(newPoint);
			}
			return this;
		} else {
			if (getPresetManager().hasMultipleRectangle()) {
				getPresetManager().endMultipleRectangle();
			}
		}

		double xDiff = newPoint.getX() - oldPoint.getX();
		double yDiff = newPoint.getY() - oldPoint.getY();

		if (fine) {
			xDiff /= 10;
			yDiff /= 10;
		}

		scroll(xDiff, yDiff);
		scrollAnimation.activate(xDiff, yDiff);
		presetManager.resetStoredViewportPosition();
		return this;
	}

	@Override
	public Control mouseUp(Position eventPoint) {
		scrollAnimation.run();

		if (getPresetManager().hasMultipleRectangle()) {
			getPresetManager().endMultipleRectangle();
		}

		return this;
	}

	@Override
	public Control pinch(Position eventPoint, double touchDist, TouchPinch pinch) {
		zoom(-2 * touchDist, eventPoint.getX(), eventPoint.getY());
		return this;
	}

	@Override
	public Control wheel(Position eventPoint, double amount, boolean fine) {
		double pix = toYPixels(-amount);
		double factor = fine ? 5 : 24;
		zoom(factor * pix / getCurrentZoom(), eventPoint.getX(), eventPoint.getY());
		return this;
	}

	private void zoom(double zoomByPixels, double x, double y) {

		double z1 = getCurrentZoom();

		NonDimension enlargeBy = toNonDimension(new Dimension(zoomByPixels, zoomByPixels));
		viewport.getNonPosition().enlargeBy(enlargeBy.getWidth());

		final NonDimension minViewportDimension = new NonDimension(50, 50 / NonMaps.theMaps.getScreenRatio());
		final NonDimension maxViewportDimension = new NonDimension(25000, 25000 / NonMaps.theMaps.getScreenRatio());
		viewport.getNonPosition().clip(minViewportDimension, maxViewportDimension);

		double z2 = getCurrentZoom();

		if (z1 != z2) {

			NonPosition pos = viewport.getNonPosition().getPosition().copy();

			double newX = x / z1 + pos.getX() - x / z2;
			double newY = y / z1 + pos.getY() - y / z2;

			viewport.moveTo(newX, newY);

			requestLayout();
			invalidate(INVALIDATION_FLAG_ZOOMED);
			presetManager.resetStoredViewportPosition();
		}
	}

	private void scroll(double d, double e) {
		NonDimension dim = toNonDimension(new Dimension(d, e));
		scroll(dim);
	}

	private void scroll(NonDimension dim) {
		if (dim.getHeight() != 0 || dim.getWidth() != 0) {
			viewport.getNonPosition().moveBy(dim.getNegated());
			double x = toXPixels(dim.getWidth());
			double y = toYPixels(dim.getHeight());
			movePixRect(x, y);
			invalidate(INVALIDATION_FLAG_SCROLLED);
		}
	}

	@Override
	public void requestLayout() {
		layoutRequested = true;
	}

	@Override
	public double getCurrentZoom() {
		if (viewport == null)
			return 1;

		return viewport.getCurrentZoom();
	}

	@Override
	public double getCurrentLevelOfDetail() {
		double zoom = getCurrentZoom() / NonMaps.devicePixelRatio;

		double minLOD = 1;
		double maxLOD = maxLevelOfDetail;

		double minZoom = 0.5;
		double maxZoom = 1;

		if (zoom < minZoom)
			return minLOD;
		else if (zoom > maxZoom)
			return maxLOD;

		double rel = (zoom - minZoom) / (maxZoom - minZoom);
		return minLOD + rel * (maxLOD - minLOD);
	}

	public void zoomToMaxLevelOfDetail() {
		double minZoom = 0.5;
		double maxZoom = 1;

		double zoom = (maxZoom - minZoom) + minZoom;
		viewport.setCurrentZoom(zoom * NonMaps.devicePixelRatio);
	}

	@Override
	public double getConturPixels() {
		return toXPixels(1);
	}

	public <T extends MapsControl & IContextMenu> T setContextMenu(T menu, NonPosition pos) {
		removeContextMenus();

		T ctxMenu = addChild(menu);
		ctxMenu.doFirstLayoutPass(getCurrentLevelOfDetail());
		pos.moveBy(
				new NonDimension(-ctxMenu.getNonPosition().getWidth() / 2, -ctxMenu.getNonPosition().getHeight() / 2));
		ctxMenu.getNonPosition().moveTo(pos);
		requestLayout();
		return ctxMenu;
	}

	private boolean removeContextMenus() {
		for (MapsControl c : getChildren()) {
			if (c instanceof IContextMenu) {
				removeChild(c);
				removeContextMenus();
				return true;
			}
		}
		return false;
	}

	public boolean handleKeyPress(final KeyDownEvent event) {
		if (Window.Location.getPort() == "8080" || Window.Location.getPort() == "8888")
			if (event.getNativeKeyCode() == com.google.gwt.event.dom.client.KeyCodes.KEY_T)
				IntegrationTests.doAllTests();

		if(event.getNativeKeyCode() == KeyCodes.KEY_SHIFT)
			isShiftDown = true;
		if(event.getNativeKeyCode() == KeyCodes.KEY_SPACE)
			isSpaceDown = true;
		if(event.getNativeKeyCode() == KeyCodes.KEY_CTRL)
			isCtrlDown = true;

		Control ctrl = recurseChildren(new ControlFinder() {

			@Override
			public boolean onWayDownFound(Control ctrl) {
				if (ctrl.onKey(event) != null) {
					event.stopPropagation();
					event.preventDefault();
					return true;
				}

				return false;
			}
		});

		return (ctrl != null);
	}

	public boolean handleKeyUp(final KeyUpEvent event) {
		if (event.getNativeKeyCode() == KeyCodes.KEY_SHIFT)
			isShiftDown = false;

		if (event.getNativeKeyCode() == KeyCodes.KEY_SPACE)
			isSpaceDown = false;

		if (event.getNativeKeyCode() == KeyCodes.KEY_CTRL)
			isCtrlDown = false;

		return true;
	}

	public boolean zoomTo(NonRect r, boolean animate) {
		double ratio = viewport.getPixRect().getWidth()
				/ (viewport.getPixRect().getHeight() - viewport.getOverlay().getBelt().getPixRect().getHeight());

		double rectRatio = r.getWidth() / r.getHeight();

		if (rectRatio > ratio) {
			double oldHeight = r.getHeight();
			r.getDimension().setHeight(r.getWidth() / ratio);
			r.getPosition().moveBy(0, -(r.getHeight() - oldHeight) / 2);
		} else if (rectRatio < ratio) {
			double oldWidth = r.getWidth();
			r.getDimension().setWidth(r.getHeight() * ratio);
			r.getPosition().moveBy(-(r.getWidth() - oldWidth) / 2, 0);
		}

		return animateViewport(r, animate);
	}

	@Override
	public Control doubleClick(Position pos) {
		if(presetManager.hasStoredViewportPosition()) {
			presetManager.resetView();
		} else {
			zoomTo(parameterEditor);
			presetManager.saveView();
		}
		return this;
	}

	public boolean animateViewport(final NonRect target, boolean animate) {
		theAnimation = null;

		double widthDiff = Math.abs(viewport.getNonPosition().getWidth() - target.getWidth());
		double leftDiff = Math.abs(viewport.getNonPosition().getLeft() - target.getLeft());
		double topDiff = Math.abs(viewport.getNonPosition().getTop() - target.getTop());

		if (toXPixels(widthDiff + leftDiff + topDiff) < 20)
			return false;

		if (!animate) {
			viewport.getNonPosition().set(target);
			calcPixRects();
			invalidate(INVALIDATION_FLAG_SCROLLED | INVALIDATION_FLAG_ZOOMED);
		} else {
			theAnimation = new Animator(750);
			viewport.getNonPosition().animateTo(target, theAnimation);
			theAnimation.addSubAnimation(0, 100, new Client() {

				@Override
				public void animate(double v) {
					requestLayout();
					invalidate(INVALIDATION_FLAG_SCROLLED | INVALIDATION_FLAG_ZOOMED);
				}
			});
		}

		return true;
	}

	@Override
	public Control drop(Position pos, DragProxy dragProxy) {
		if (dragProxy.getOrigin() instanceof DeveloperSettings) {
			NonPosition np = toNonPosition(dragProxy.getPixRect().getPosition());
			getSettings().getNonPosition().moveTo(np);
			requestLayout();
			return this;
		}
		return super.drop(pos, dragProxy);
	}

	public void scrollToShow(MapsControl mapsControl) {
		double margin = Millimeter.toPixels(20);
		Rect controlPos = mapsControl.getPixRect().getReducedBy(-margin);
		NonPosition leftTop = toNonPosition(controlPos.getLeftTop());
		NonPosition rightBottom = toNonPosition(controlPos.getRightBottom());
		Rect vpPixRect = getViewport().getPixRectWithoutBelt();
		NonRect vpRect = new NonRect(toNonPosition(vpPixRect.getPosition()), toNonDimension(vpPixRect.getDimension()));

		double xOffset = 0;
		double yOffset = 0;

		if (leftTop.getX() < vpRect.getLeft())
			xOffset = leftTop.getX() - vpRect.getLeft();

		if (rightBottom.getX() > vpRect.getRight())
			xOffset = rightBottom.getX() - vpRect.getRight();

		if (leftTop.getY() < vpRect.getTop())
			yOffset = leftTop.getY() - vpRect.getTop();

		if (rightBottom.getY() > vpRect.getBottom())
			yOffset = rightBottom.getY() - vpRect.getBottom();

		if (xOffset != 0 || yOffset != 0) {
			vpRect.moveBy(new NonDimension(xOffset, yOffset));
			animateViewport(vpRect, true);
		}
	}

	@Override
	public Control onContextMenu(Position pos) {
		if (getPresetManager().isInStoreSelectMode())
			return null;

		boolean showContextMenus = SetupModel.get().localSettings.contextMenus.getValue() == BooleanValues.on;
		if (showContextMenus) {
			Overlay o = getViewport().getOverlay();
			return o.setContextMenu(pos, new PresetManagerContextMenu(o));
		}
		return super.onContextMenu(pos);
	}

	public boolean isShiftDown() {
		return isShiftDown;
	}

	public boolean isSpaceDown() {
		return isSpaceDown;
	}

	public boolean isCtrlDown() {
		return isCtrlDown;
	}

	public void setShiftDown(boolean s) {
		isShiftDown = s;
	}

	public void setCtrlDown(boolean s) {
		isCtrlDown = s;
	}

	@Override
	public boolean isDraggingControl() {
		return false;
	}
}
