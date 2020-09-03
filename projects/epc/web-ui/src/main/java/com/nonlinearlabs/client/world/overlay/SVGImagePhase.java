package com.nonlinearlabs.client.world.overlay;

import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.dom.client.ImageElement;
import com.google.gwt.user.client.ui.Image;
import com.google.gwt.xml.client.Document;
import com.google.gwt.xml.client.Element;
import com.google.gwt.xml.client.Node;
import com.google.gwt.xml.client.NodeList;
import com.google.gwt.xml.client.XMLParser;
import com.nonlinearlabs.client.Millimeter;
import com.nonlinearlabs.client.NonMaps;
import com.nonlinearlabs.client.ServerProxy.DownloadHandler;
import com.nonlinearlabs.client.world.Position;
import com.nonlinearlabs.client.world.Rect;

public class SVGImagePhase extends OverlayControl {

	private ImageElement face = null;
	private double imgWidthInMM = 0;
	private double imgHeightInMM = 0;

	public SVGImagePhase(SVGImage parent, String imageName) {
		super(parent);
		preloadImage("images/" + imageName);
	}

	private static native String base64Encode(String v) /*-{
														return btoa(v);
														}-*/;

	private static native String base64Decode(String v) /*-{
														return atob(v);
														}-*/;

	private void preloadImage(final String imageName) {
		final Image img = new Image();
		face = ImageElement.as(img.getElement());

		getNonMaps().getServerProxy().downloadFile(imageName, new DownloadHandler() {

			@Override
			public void onFileDownloaded(String text) {
				face.setSrc(getImageAsSrc(text));
				requestLayout();
			}

			@Override
			public void onError() {
			}
		});
	}

	protected String getImageAsSrc(String text) {
		Document dom = XMLParser.parse(text);
		NodeList svgs = dom.getElementsByTagName("svg");
		if (svgs.getLength() > 0)
			return setSVGDimension(svgs);

		return text;
	}

	protected String setSVGDimension(NodeList svgs) {
		Node svg = svgs.item(0);
		Element svgElement = (Element) svg;

		String viewbox = svgElement.getAttribute("viewBox");
		String splits[] = viewbox.split(" ");
		imgWidthInMM = parseWidthInMM(splits[2]);
		imgHeightInMM = parseHeightInMM(splits[3]);
		svgElement.setAttribute("width", imgWidthInMM + "mm");
		svgElement.setAttribute("height", imgHeightInMM + "mm");
		return "data:image/svg+xml;base64," + base64Encode(svg.toString());
	}

	protected double parseWidthInMM(String str) {
		return calcSVGDimensionToPixels(Double.parseDouble(str));
	}

	protected double parseHeightInMM(String str) {
		return calcSVGDimensionToPixels(Double.parseDouble(str));
	}

	static public double calcSVGDimensionToPixels(double svgDim) {
		double mmPerPix = NonMaps.devicePixelRatio / Millimeter.toPixels(1);
		return mmPerPix * svgDim;
	}

	@Override
	public void calcPixRect(Position parentsReference, double currentZoom) {
		Rect newPos = getRelativePosition().copy();
		double w = Millimeter.toPixels(imgWidthInMM);
		double h = Millimeter.toPixels(imgHeightInMM);
		newPos.setWidth(w);
		newPos.setHeight(h);
		newPos.getPosition().moveBy(parentsReference);
		newPos.round();
		newPos.moveBy(0.5, 0.5);
		setPixRect(newPos);

	}

	@Override
	public void draw(Context2d ctx, Context2d overlay, int invalidationMask) {
		if (face != null) {
			Rect r = getPixRect();
			ctx.drawImage(face, r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
		}
	}

	public double getImgWidth() {
		return Millimeter.toPixels(imgWidthInMM);
	}

	public double getImgHeight() {
		return Millimeter.toPixels(imgHeightInMM);
	}
}
