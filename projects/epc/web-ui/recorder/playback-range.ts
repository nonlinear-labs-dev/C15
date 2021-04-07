
class PlaybackRange {
    constructor(private waveForm: Waveform) {
    }

    min(): number {
        if (this.waveForm.bars.count() == 0)
            return 0;

        return Math.max(Math.min(this.one, this.other), this.waveForm.bars.first().id);
    }

    max(): number {
        if (this.waveForm.bars.count() == 0)
            return 0;

        return Math.min(Math.max(this.one, this.other), this.waveForm.bars.last().id);
    }

    one = 0;
    other = 0;
}