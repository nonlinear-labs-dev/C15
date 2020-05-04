package com.nonlinearlabs.client.world.overlay;

import java.util.HashMap;
import java.util.LinkedList;

import com.google.gwt.canvas.client.Canvas;
import com.google.gwt.canvas.dom.client.Context2d;
import com.google.gwt.canvas.dom.client.Context2d.TextAlign;
import com.nonlinearlabs.client.world.Rect;

class LineWrappingLabel extends OverlayControl {

	private HashMap<String, Double> wordToLen = new HashMap<String, Double>();
	private LinkedList<String> lines = new LinkedList<String>();

	LineWrappingLabel(OverlayLayout parent, String text) {
		super(parent);
		setText(text);
	}

	public double getWidth() {
		return 100;
	}

	public double getLeftMargin() {
		return 20;
	}

	public double getHeight() {
		return lines.size() * getLineHeight();
	}

	private double getLineHeight() {
		return getFontHeight() + getLineSpace();
	}

	private double getLineSpace() {
		return 5;
	}

	@Override
	public void draw(Context2d ctx, int invalidationMask) {
		Rect pixRect = getPixRect();

		double fontHeightInPixels = getFontHeight();

		ctx.setTextAlign(TextAlign.LEFT);
		ctx.setFillStyle(getColorFont().toString());
		ctx.setStrokeStyle(getColorFont().toString());
		ctx.setFont(fontHeightInPixels + "px 'SSP-LW25'");

		int lineNumber = 1;

		for (String line : lines) {
			ctx.fillText(line, pixRect.getLeft() + getLeftMargin(),
					pixRect.getTop() + lineNumber * getLineHeight() - fontHeightInPixels / 2);
			lineNumber++;
		}
	}

	private double getFontHeight() {
		return 10;
	}

	public void setText(String text) {
		lines.clear();
		Canvas canvas = Canvas.createIfSupported();
		Context2d context = canvas.getContext2d();
		context.setFont(getFontHeight() + "px 'SSP-LW25'");
		String words[] = text.split("\\s+");

		double pixLeftInCurrentLine = getAvailableLineWidth();
		double spaceWidth = context.measureText(" ").getWidth();

		StringBuilder currentLine = new StringBuilder();

		for (String word : words) {
			if (!wordToLen.containsKey(word)) {
				double width = context.measureText(word).getWidth();
				wordToLen.put(word, width);
			}

			double wordWidth = wordToLen.get(word);

			if (pixLeftInCurrentLine < wordWidth) {
				lines.add(currentLine.toString());
				currentLine = new StringBuilder();
				pixLeftInCurrentLine = getAvailableLineWidth();
			}

			pixLeftInCurrentLine -= wordWidth;
			pixLeftInCurrentLine -= spaceWidth;
			currentLine.append(word).append(" ");
		}

		String lastLine = currentLine.toString();

		if (!lastLine.isEmpty())
			lines.add(lastLine);

		requestLayout();
	}

	private double getAvailableLineWidth() {
		return getWidth() - getLeftMargin();
	}
}
