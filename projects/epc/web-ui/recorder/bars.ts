const singleBarLength = Math.round((4096 / 48000) * 1000 * 1000 * 1000);

class Bar {
    id: number = 0;
    max: number = 0;
    recordTime: number = 0;
}

class Bars {
    numIntegrals = 32;

    constructor(private barLength: number) {
        for (var i = 0; i < this.numIntegrals; i++)
            this.bars.push(new Array<number>());
    }

    add(bar: Bar) {
        if (this.firstId == Number.MIN_SAFE_INTEGER) {
            this.addNext(bar);
            return;
        }

        var expectedId = this.firstId + this.bars[0].length;
        var lastMax = this.last().max;

        for (var id = expectedId; id < bar.id; id++) {
            var toAdd = { id: id, max: lastMax, recordTime: this.lastTimestamp + this.barLength };
            this.addNext(toAdd);
        }

        this.addNext(bar);
    }

    private addNext(bar: Bar) {
        this.bars[0].push(bar.max);

        if (this.firstId == Number.MIN_SAFE_INTEGER)
            this.firstId = bar.id;

        var expectedTimestamp = this.lastTimestamp + this.barLength;
        var timeDiff = Math.abs(expectedTimestamp - bar.recordTime);
        var ms = 1000 * 1000; // accept 1 ms per frame

        if (timeDiff > ms)
            this.timestamps.set(bar.id, bar.recordTime);

        this.lastTimestamp = bar.recordTime;

        var idx = this.bars[0].length - 1;

        for (var i = 1; i < this.numIntegrals; i++) {
            idx = Math.floor(idx / 2);

            if (this.bars[i].length <= idx)
                this.bars[i].push(0);

            this.bars[i][idx] = Math.max(this.bars[i][idx], bar.max);
        }

        assert(Math.abs(this.last().recordTime - this.lastTimestamp) < this.barLength);
    }

    remove() {
        if (!this.empty()) {
            this.bars[0].splice(0, 1);

            if (this.empty()) {
                this.clear();
                return;
            }

            if (this.timestamps.has(this.firstId)) {
                var ts = this.timestamps.get(this.firstId)!;
                this.timestamps.delete(this.firstId);

                this.firstId++;

                if (!this.timestamps.has(this.firstId))
                    this.timestamps.set(this.firstId, ts + this.barLength);
            }
            else {
                this.firstId++;
            }

            for (var i = 0; i < this.numIntegrals - 1; i++) {
                var srcArray = this.bars[i];
                var tgtArray = this.bars[i + 1];

                if (tgtArray.length > Math.round(srcArray.length / 2))
                    tgtArray.splice(0, 1);

                if (srcArray.length > 1 && srcArray.length % 2 == 0)
                    tgtArray[0] = Math.max(srcArray[0], srcArray[1]);
                else
                    tgtArray[0] = srcArray[0];
            }
        }
    }

    removeUntil(id: number): boolean {
        var ret = false;
        while (!this.empty() && this.firstId <= id) {
            this.remove();
            ret = true;
        }
        return ret;
    }

    clear() {
        this.bars = new Array<Array<number>>();
        this.firstId = Number.MIN_SAFE_INTEGER;
        this.timestamps = new Map<number, number>();
        this.lastTimestamp = Number.MIN_SAFE_INTEGER;

        for (var i = 0; i < this.numIntegrals; i++)
            this.bars.push(new Array<number>());
    }

    count(): number {
        return this.bars[0].length;
    }

    empty(): boolean {
        return this.count() == 0;
    }

    get(id: number): Bar | null {
        var idx = id - this.firstId;

        if (this.bars[0].length <= idx)
            return null;

        var ts = 0;
        var max = idx >= 0 && idx < this.bars[0].length ? this.bars[0][idx] : 0;
        var largestIdSoFar = -1;

        for (let [k, v] of this.timestamps) {
            if (k <= id && k > largestIdSoFar) {
                ts = v + (id - k) * this.barLength;
                largestIdSoFar = k;
            }
        }

        return { id: id, max: max, recordTime: ts };
    }

    getMax(id: number, zoom: number): number {
        var idx = id - this.firstId;

        if (idx < 0)
            return 0;

        for (var i = 0; i < this.numIntegrals; i++) {
            if (zoom < 2) {
                return this.bars[i][Math.round(idx)];
            }

            zoom /= 2;
            idx = Math.floor(idx / 2);
        }
        return 0;
    }

    first(): Bar {
        return this.get(this.firstId) || { id: 0, max: 0, recordTime: 0 };
    }

    last(): Bar {
        return this.get(this.firstId + this.bars[0].length - 1) || { id: 0, max: 0, recordTime: 0 };
    }


    firstId: number = Number.MIN_SAFE_INTEGER;

    private bars = new Array<Array<number>>();
    private timestamps = new Map<number, number>();
    private lastTimestamp = Number.MIN_SAFE_INTEGER;
}
