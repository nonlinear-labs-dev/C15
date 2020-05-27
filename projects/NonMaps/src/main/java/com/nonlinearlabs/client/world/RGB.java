package com.nonlinearlabs.client.world;

public class RGB {

	int r;
	int g;
	int b;

	public RGB(String color) {
		if (isHex(color))
			fromHex(color);
		else
			assert (false);
	}

	public RGB(int r, int g, int b) {
		this.r = clip(r);
		this.g = clip(g);
		this.b = clip(b);
	}

	public RGB clone() {
		return new RGB(r, g, b);
	}

	private int clip(int in) {
		return Math.max(0, Math.min(255, in));
	}

	public String toString() {
		return "rgb(" + r + "," + g + "," + b + ")";
	}

	private boolean isHex(String color) {
		return color.startsWith("#") && color.length() == 7;
	}

	private void fromHex(String color) {
		String noHashtag = color.substring(1);
		this.r = clip(Integer.valueOf(noHashtag.substring(0, 1), 16));
		this.g = clip(Integer.valueOf(noHashtag.substring(2, 3), 16));
		this.b = clip(Integer.valueOf(noHashtag.substring(4, 5), 16));
	}

	public RGB adjust(double d) {
		return new RGB((int)(r * d), (int)(g * d), (int)(b * d));
	}

	public RGB brighter(int amount) {
		return new RGB(r + amount, g + amount, b + amount);
	}

	public RGB darker(int amount) {
		return brighter(-amount);
	}

	public static RGB blue() {
		return new RGB(107, 115, 153);
	}

	public static RGB black() {
		return new RGB(0, 0, 0);
	}

	public static RGB white() {
		return new RGB(255, 255, 255);
	}

	public static RGB red() {
		return new RGB(255, 0, 0);
	}

	public static RGB yellow() {
		return new RGB(255, 255, 0);
	}

	public static RGB lightGray() {
		return new RGB(155, 159, 163);
	}

	public static RGB changedBeltBorder() {
		return new RGB(196, 205, 145);
	}

	public static RGB changedBorder() {
		return new RGB(217, 231, 121);
	}

	public static RGB changedText() {
		return new RGB(126, 131, 95);
	}

	public static RGB lighterGray() {
		return new RGB(190, 190, 190);
	}

	public static RGB floatingWindowHeaderBorder() {
		return new RGB(118, 130, 154);
	}

	public static RGB floatingWindowHeaderBackground() {
		return new RGB(22, 22, 30);
	}

	public RGBA withAlpha(double alpha) {
		return new RGBA(this, alpha);
	}

	public int getRed() {
		return r;
	}

	public int getGreen() {
		return g;
	}

	public int getBlue() {
		return b;
	}

}
