
class PlaybackRange {

    constructor(private c15: C15ProxyIface) {
    }

    min(): number {
        if (this.c15.getBars().count() == 0)
            return 0;

        return Math.max(Math.min(this.one, this.other), this.c15.getBars().first().id);
    }

    max(): number {
        if (this.c15.getBars().count() == 0)
            return 0;

        return Math.min(Math.max(this.one, this.other), this.c15.getBars().last().id);
    }

    empty(): boolean {
        return this.one == this.other;
    }

    one = 0;
    other = 0;
}