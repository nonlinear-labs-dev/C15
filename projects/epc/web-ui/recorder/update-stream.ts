
class TimingInfo {
    serverTime: number = 0;
    localTime: number = 0;
}

function assert(b: boolean) {
    if (!b)
        throw new Error('assertion failes');
}

class MockUpdateStream {
    connect(display: UI): void {
        console.log("connect");
        this.ui = display;
        this.bars.clear();

        this.timingInfo.serverTime = this.time;
        this.timingInfo.localTime = this.time;


        // this.addBars(10000);
        // setInterval(() => this.addBars(10), 200);

        var b1 = new Bar();
        b1.id = 0;
        b1.max = 0;
        b1.recordTime = 0;
        this.bars.add(b1);

        var b2 = new Bar();
        b2.id = 9;
        b2.max = 0;
        b2.recordTime = barLength * 9;
        this.bars.add(b2);
        assert(this.bars.count() == 10);

        var b3 = new Bar();
        b3.id = 10;
        b3.max = 0;
        b3.recordTime = barLength * 10;
        this.bars.add(b3);
        assert(this.bars.count() == 11);

        var b4 = new Bar();
        b4.id = 11;
        b4.max = 5;
        b4.recordTime = barLength * 11;
        this.bars.add(b4);
        assert(this.bars.count() == 12);

        this.bars.remove();
        assert(this.bars.count() == 11);
        assert(this.bars.first().recordTime == barLength);

        this.ui!.updateFirstAndLastFrame(this.bars.first().recordTime, this.bars.last().recordTime);
        this.ui!.updateMemoryUsage(1234567, 500 * 1024 * 1024);
        this.ui!.updateTransportStates(false, true, this.bars.get(this.bars.count() / 2).recordTime);
    }

    addBars(n: number) {
        for (var i = 0; i < n; i++) {
            var b = new Bar();
            b.id = this.id++;
            b.max = Math.max(Math.sin(this.id / 10) * Math.sin(this.id / 100) * Math.sin(this.id / 1000) * Math.sin(this.id / 10000) * 256);

            if (b.max < 5)
                b.max = 0;

            b.recordTime = this.time;
            this.time += barLength;
            this.bars.add(b);
        }
        this.ui!.update();
    }

    bars: Bars = new Bars();
    timingInfo: TimingInfo = new TimingInfo();

    private id = 3;
    private time = new Date().getTime() * 1000 * 1000;
    private ui: UI | null = null;
}

class UpdateStream {
    connect(display: UI): void {
        console.log("connect");
        this.ui = display;
        this.bars.clear();
        this.socket = new WebSocket("ws://" + hostName + wsPort);
        this.socket.onopen = (event) => this.update();
        this.socket.onerror = (event) => this.retry();
        this.socket.onclose = (event) => this.retry();
        this.socket.onmessage = (event) => this.readMessage(event.data);
    }

    private update(): void {
        this.messageHandler = (e) => this.processInfo(e);

        try {
            this.socket!.send(JSON.stringify({ "get-info": {} }));
        }
        catch (err) {
            this.retry();
        }
    }

    private processInfo(info: any) {
        this.timingInfo.serverTime = info.time;
        this.timingInfo.localTime = Date.now() * 1000 * 1000;

        if (!info.storage) {
            this.ui!.update();
            this.scheduleUpdate();
            return;
        }

        this.ui!.updateFirstAndLastFrame(info.storage.first.recordTime, info.storage.last.recordTime);
        this.ui!.updateMemoryUsage(info.storage.memUsage.current, info.storage.memUsage.max);
        this.ui!.updateTransportStates(info.recorder.paused, info.player.paused, info.player.pos);

        // remove outdated bars
        this.updateWaveform = this.bars.removeUntil(info.storage.first.id) || this.updateWaveform;

        // query new bars
        var nextId = info.storage.first.id;

        if (this.bars.count() > 0)
            nextId = this.bars.last().id + 1;

        if (info.storage.last.id - nextId < 1) {
            this.ui!.update();
            this.scheduleUpdate();
            return;
        }

        this.messageHandler = (e) => this.processQuery(e);
        this.socket!.send(JSON.stringify({ "query-frames": { "begin": nextId, "end": info.storage.last.id } }));
    }

    private scheduleUpdate() {
        if (this.updateTimer == -1) {
            this.updateTimer = setTimeout(() => {
                this.updateTimer = -1;
                this.update();
            }, 200);
        }
    }

    private processQuery(info: any) {
        info.forEach((a: Bar) => {
            this.bars.add(a);
            this.updateWaveform = true;
        });

        if (this.updateWaveform) {
            this.updateWaveform = false;
            this.ui!.update();
        }

        this.scheduleUpdate();
    }

    private retry(): void {
        if (this.retryTimer == -1) {
            console.log("retry connection");
            this.retryTimer = setTimeout(() => {
                this.retryTimer = -1;
                this.connect(this.ui!);
            }, 200);
        }
    }

    private readMessage(data: any): void {
        var reader = new FileReader()
        reader.onload = () => {
            this.messageHandler!(JSON.parse(reader.result as string));
        }
        reader.readAsText(data);
    }

    bars: Bars = new Bars();
    timingInfo: TimingInfo = new TimingInfo();

    private socket: WebSocket | null = null;
    private ui: UI | null = null;
    private messageHandler: ((e: any) => void) | null = null;
    private updateWaveform = false;
    private retryTimer = -1;
    private updateTimer = -1;
}