const singleBarLength = Math.round((4096 / 48000) * 1000 * 1000 * 1000);
type uint8 = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 | 59 | 60 | 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 | 72 | 73 | 74 | 75 | 76 | 77 | 78 | 79 | 80 | 81 | 82 | 83 | 84 | 85 | 86 | 87 | 88 | 89 | 90 | 91 | 92 | 93 | 94 | 95 | 96 | 97 | 98 | 99 | 100 | 101 | 102 | 103 | 104 | 105 | 106 | 107 | 108 | 109 | 110 | 111 | 112 | 113 | 114 | 115 | 116 | 117 | 118 | 119 | 120 | 121 | 122 | 123 | 124 | 125 | 126 | 127 | 128 | 129 | 130 | 131 | 132 | 133 | 134 | 135 | 136 | 137 | 138 | 139 | 140 | 141 | 142 | 143 | 144 | 145 | 146 | 147 | 148 | 149 | 150 | 151 | 152 | 153 | 154 | 155 | 156 | 157 | 158 | 159 | 160 | 161 | 162 | 163 | 164 | 165 | 166 | 167 | 168 | 169 | 170 | 171 | 172 | 173 | 174 | 175 | 176 | 177 | 178 | 179 | 180 | 181 | 182 | 183 | 184 | 185 | 186 | 187 | 188 | 189 | 190 | 191 | 192 | 193 | 194 | 195 | 196 | 197 | 198 | 199 | 200 | 201 | 202 | 203 | 204 | 205 | 206 | 207 | 208 | 209 | 210 | 211 | 212 | 213 | 214 | 215 | 216 | 217 | 218 | 219 | 220 | 221 | 222 | 223 | 224 | 225 | 226 | 227 | 228 | 229 | 230 | 231 | 232 | 233 | 234 | 235 | 236 | 237 | 238 | 239 | 240 | 241 | 242 | 243 | 244 | 245 | 246 | 247 | 248 | 249 | 250 | 251 | 252 | 253 | 254;

class Bar {
    id: number = 0;
    max: uint8 = 0;
    recordTime: number = 0;
}

class Bars2 {



    constructor() {
        for (var i = 0; i < 32; i++)
            this.bars.push(new Array<uint8>());
    }

    add(bar: Bar) {
        this.bars[0].push(bar.max);

    }

    bars = new Array<Array<uint8>>();
    firstId: number = -1;
    timestamps = new Map<number, number>();
}

class Bars {
    constructor(private barLength: number, private integrals: Integrals | null) {
    }

    removeUntil(id: number): boolean {
        var ret = false;

        while (this.count() > 0) {
            if (this.first().id >= id)
                break;

            this.remove();
            ret = true;
        }
        return ret;
    }

    add(bar: Bar) {
        var reuseLastBar = false;

        if (this.bars.length > 0) {
            var last = this.bars[this.bars.length - 1];
            var idDiff = bar.id - last.id;
            var timeDiff = bar.recordTime - last.recordTime;
            var diffFromExpected = Math.abs(timeDiff - idDiff * this.barLength);
            var ms = idDiff * 1000 * 1000; // accept 1 ms per frame
            reuseLastBar = last.max == bar.max && diffFromExpected < ms;
        }

        if (!reuseLastBar)
            this.bars.push(bar);

        this.lastId = bar.id;

        var numBars = this.count();
        this.integrals?.setBack(numBars > 1 ? this.get(this.last().id - 1) : null,
            numBars > 0 ? this.last() : null);

    }

    remove() {
        if (this.count() == 0)
            return;

        if (this.count() == 1) {
            this.clear();
            return;
        }

        var nextId = (this.bars.length > 1) ? this.bars[1].id : this.lastId;
        var firstBar = this.bars[0];

        if (nextId == (firstBar.id + 1)) {
            this.bars.splice(0, 1);
        }
        else {
            firstBar.id++;
            firstBar.recordTime += this.barLength;
        }

        var numBars = this.count();
        this.integrals?.setFront(numBars > 0 ? this.first() : null, numBars > 1 ? this.get(this.first().id + 1) : null);
    }

    clear() {
        this.bars = new Array<Bar>();
        this.lastId = -1;
        this.integrals?.clear();
    }

    count(): number {
        if (this.bars.length != 0)
            return 1 + (this.lastId - this.first().id);

        return 0;
    }

    empty(): boolean {
        return this.lastId == -1;
    }

    get(id: number): Bar {
        var b = this.searchLessOrEqual(id, 0, this.bars.length - 1);
        var dist = id - b.id;
        var ret = new Bar();
        ret.id = id;
        ret.max = b.max;
        ret.recordTime = b.recordTime + dist * this.barLength;
        return ret;
    }

    getMax(idx: number, zoom: number): number {
        if (zoom > 2 && this.integrals)
            return this.integrals.getMax(idx, zoom);

        return this.get(idx).max;
    }

    first(): Bar {
        return this.bars[0];
    }

    last(): Bar {
        return this.get(this.count() + this.bars[0].id - 1);
    }

    private searchLessOrEqual(idx: number, start: number, end: number): Bar {
        var diff = end - start;

        if (diff <= 3) {
            for (var i = end; i >= start; i--) {
                if (this.bars[i].id <= idx)
                    return this.bars[i];
            }
            return this.bars[start];
        }
        else {
            var mid = Math.round(start + diff / 2);
            var b = this.bars[mid];
            if (b.id == idx)
                return b;
            else if (b.id > idx)
                return this.searchLessOrEqual(idx, start, mid);
            else
                return this.searchLessOrEqual(idx, mid, end);
        }
    }

    getLastStoredBarRef(): Bar {
        return this.bars[this.bars.length - 1];
    }

    private bars = Array<Bar>();
    private lastId = -1;
}

class Integrals {
    constructor(private factor: number) {
        if (factor < 8192)
            this.next = new Integrals(factor * 2);
    }

    getMax(idx: number, zoom: number): number {
        if (zoom > (2 * this.factor) && this.next)
            return this.next.getMax(idx, zoom);

        return this.bars.get(Math.floor(idx / zoom)).max;
    }


    private getFirstMax(first: Bar, second: Bar | null): number {
        if (second == null)
            return first.max;

        if (Math.floor(first.id / 2) == Math.floor(second.id / 2))
            return Math.max(first.max, second.max);

        return first.max;
    }

    private getLastMax(before: Bar | null, last: Bar): number {
        if (before == null)
            return last.max;

        if (Math.floor(last.id / 2) == Math.floor(before.id / 2))
            return Math.max(last.max, before.max);

        return last.max;
    }

    setFront(first: Bar | null, second: Bar | null) {
        assert(first != null || (first == null && second == null));

        if (first == null) {
            this.clear();
            return;
        }

        var id = Math.floor(first.id / 2);

        while (this.bars.count() && this.bars.first().id != id)
            this.bars.remove();

        var numBars = this.bars.count();

        if (numBars) {
            var firstExisting = this.bars.first();
            assert(firstExisting.id == id);
            firstExisting.max = this.getFirstMax(first, second);
            firstExisting.recordTime = 0;
        }

        this.next?.setFront(numBars > 0 ? this.bars.first() : null, numBars > 1 ? this.bars.get(this.bars.first().id + 1) : null);
    }

    setBack(before: Bar | null, last: Bar | null) {
        assert(last != null || (before == null && last == null));

        if (last == null) {
            this.clear();
            return;
        }

        var bar = new Bar();
        bar.id = Math.floor(last.id / 2);
        bar.max = this.getLastMax(before, last);
        bar.recordTime = 0;

        if (!this.bars.empty() && this.bars.last().id == bar.id) {
            this.bars.getLastStoredBarRef().max = Math.max(this.bars.last().max, bar.max);
        }
        else {
            this.bars.add(bar);
        }

        var numBars = this.bars.count();
        this.next?.setBack(numBars > 1 ? this.bars.get(this.bars.last().id - 1) : null, numBars > 0 ? this.bars.last() : null);
    }

    clear() {
        this.bars = new Bars(0, null);

        if (this.next)
            this.next.clear();
    }

    private next: Integrals | null = null;
    private bars = new Bars(0, null);
}