package com.nonlinearlabs.NonMaps.client.world.overlay.layouter;

import java.util.ArrayList;

public class HarmonicLayouter2 {

	static int invalidHideGroup = 100000;

	public HarmonicLayouter2() {
	}

	public class Result {
		public Record record;
		public double width;
	}

	public class Record {
		public Object attached;
		int hideGroup;
		double minWidth;
		double maxWidth;
		int prio;
	}

	public void push(Object attached, double minWidth, double maxWidth, int hideGroup, int prio) {
		Record r = new Record();
		r.attached = attached;
		r.hideGroup = hideGroup;
		r.minWidth = minWidth;
		r.maxWidth = Math.max(minWidth, maxWidth);
		r.prio = prio;
		records.add(r);
	}

	public Result[] layout(final double availableWidth) {
		Result[] ret = createResultArray();

		hideAdMuchAsNeeded(ret, availableWidth);

		spreadAvailableWidth(ret, availableWidth);
		spreadAvailableWidth(ret, availableWidth);
		spreadAvailableWidth(ret, availableWidth);

		return ret;
	}

	private void spreadAvailableWidth(Result[] result, double availableWidth) {
		double actualWidth = getCurrentWidth(result);
		double amountToSpread = availableWidth - actualWidth;

		if (amountToSpread > 0) {
			double weightSum = 0;

			for (Result r : result) {
				if (r.width > 0 && r.width < r.record.maxWidth) {
					weightSum += r.record.prio;
				}
			}

			for (Result r : result) {
				if (r.width > 0 && r.width < r.record.maxWidth) {
					double amountForElement = amountToSpread * r.record.prio / weightSum;
					r.width = Math.min(r.record.maxWidth, r.width + amountForElement);
				}
			}
		}
	}

	private void hideAdMuchAsNeeded(Result[] result, double availableWidth) {
		double width = getMinWidth(result);

		while (width > availableWidth) {
			int hide = getSmallestVisibleHideGroup(result);

			if (hide == invalidHideGroup)
				throw new IllegalArgumentException();

			for (Result r : result) {
				if (r.record.hideGroup == hide)
					r.width = 0;
			}

			width = getMinWidth(result);
		}
	}

	private int getSmallestVisibleHideGroup(Result[] result) {
		int group = invalidHideGroup;

		for (Result r : result) {
			if (r.width > 0 && r.record.hideGroup > 0 && r.record.hideGroup < group)
				group = r.record.hideGroup;
		}
		return group;
	}

	private Result[] createResultArray() {
		Result result[] = new Result[records.size()];

		for (int i = 0; i < records.size(); i++) {
			result[i] = new Result();
			result[i].record = records.get(i);
			result[i].width = result[i].record.minWidth;
		}
		return result;
	}

	private double getMinWidth(Result[] result) {
		double actualWidth = 0;

		for (Result r : result) {
			if (r.width != 0)
				actualWidth += r.record.minWidth;
		}
		return actualWidth;
	}

	private double getCurrentWidth(Result[] result) {
		double actualWidth = 0;

		for (Result r : result) {
			actualWidth += r.width;
		}
		return actualWidth;
	}

	private ArrayList<Record> records = new ArrayList<Record>();
}
