
class TimingInfo {
    serverTime: number = 0;
    localTime: number = 0;
}

function assert(b: boolean) {
    if (!b)
        throw new Error('assertion failed');
}

class MockUpdateStream {
    constructor(private c15: C15ProxyIface, addBars: boolean) {
        console.log("connect");
        this.bars.clear();

        this.timingInfo.serverTime = this.time;
        this.timingInfo.localTime = this.time;

        if (addBars)
            this.addBars(5000, true);

        this.timer = setInterval(() => this.addBars(1, false), 85);

        c15.updateMemoryUsage(1234567, 500 * 1024 * 1024);
        c15.updateTransportStates(false, true, this.bars.firstId + this.bars.count() / 2);
    }

    stop() {
        clearInterval(this.timer);
    }

    addBars(n: number, force: boolean) {
        if (this.c15.getRecordingState() != TransportState.Paused || force) {
            for (var i = 0; i < n; i++) {
                var b = new Bar();
                b.id = this.id++;
                b.max = Math.abs(Math.sin(this.id / 10) * Math.sin(this.id / 100) * Math.sin(this.id / 1000) * Math.sin(this.id / 10000) * 256);

                if (b.max < 5)
                    b.max = 0;

                b.recordTime = this.time;
                this.time += singleBarLength;
                this.bars.add(b);
            }
            this.c15.synced();
        }
    }

    bars: Bars = new Bars(singleBarLength);
    timingInfo: TimingInfo = new TimingInfo();

    private id = 3;
    private timer = 0;
    private time = new Date().getTime() * 1000 * 1000;
}

class UpdateStream {
    constructor(private c15: C15ProxyIface) {
        this.connect();
    }

    private connect() {
        console.log("connect");
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
            this.c15.synced();
            this.scheduleUpdate();
            return;
        }

        this.c15.updateMemoryUsage(info.storage.memUsage.current, info.storage.memUsage.max);
        this.c15.updateTransportStates(info.recorder.paused, info.player.paused, info.player.pos);

        // remove outdated bars
        this.updateWaveform = this.bars.removeUntil(info.storage.first.id) || this.updateWaveform;

        // query new bars
        var nextId = info.storage.first.id;

        if (this.bars.count() > 0)
            nextId = this.bars.last()!.id + 1;

        if (info.storage.last.id - nextId < 1) {
            this.c15.synced();
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
            this.c15.synced();
        }

        this.scheduleUpdate();
    }

    private retry(): void {
        if (this.retryTimer == -1) {
            console.log("retry connection");
            this.retryTimer = setTimeout(() => {
                this.retryTimer = -1;
                this.connect();
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

    bars: Bars = new Bars(singleBarLength);
    timingInfo: TimingInfo = new TimingInfo();

    private socket: WebSocket | null = null;
    private messageHandler: ((e: any) => void) | null = null;
    private updateWaveform = false;
    private retryTimer = -1;
    private updateTimer = -1;
}