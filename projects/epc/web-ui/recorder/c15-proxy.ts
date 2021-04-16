enum TransportState {
    Playing,
    Recording,
    Paused
}

abstract class C15ProxyIface {
    abstract download(from: number, to: number): void;
    abstract getBars(): Bars;
    abstract buildTime(lastTime: number | undefined): string;
    abstract setPlaybackPosition(playPos: number): void;
    abstract reset(): void;
    abstract toggleRecording(): void;
    abstract pausePlayback(): void;
    abstract startPlayback(): void;
    abstract undo(): void;

    connect(ui: UI) {
        this.ui = ui;
    }

    synced() {
        if (this.ui)
            this.ui.update();
    }

    togglePlayback(): void {
        if (this.getPlaybackState() == TransportState.Playing)
            this.pausePlayback();
        else
            this.startPlayback();
    }

    updateTransportStates(recPaused: boolean, playPaused: boolean, playPos: number): void {
        this.recordPaused = recPaused;
        this.playbackPaused = playPaused;
        this.currentPlayPosition = playPos;
    }

    updateMemoryUsage(currentUsage: number, maxUsage: number): void {
        this.currentMemoryUsage = currentUsage;
        this.maxMemoryUsage = maxUsage;
    }

    getCurrentMemoryUsage(): number {
        return this.currentMemoryUsage;
    }

    getMaxMemoryUsage(): number {
        return this.maxMemoryUsage;
    }

    getPlaybackState(): TransportState {
        return this.playbackPaused ? TransportState.Paused : TransportState.Playing;
    }

    getRecordingState(): TransportState {
        return this.recordPaused ? TransportState.Paused : TransportState.Recording;
    }

    getCurrentPlayPosition(): number {
        return this.currentPlayPosition;
    }


    static create(): C15ProxyIface {
        var mock = location.search.includes("mock-audio-data");
        return mock ? new C15ProxyMock() : new C15Proxy();
    }

    protected ui: UI | null = null;
    protected currentMemoryUsage = 0;
    protected maxMemoryUsage = 0;
    protected recordPaused = true;
    protected playbackPaused = true;
    protected currentPlayPosition = 0;
}

class C15ProxyMock extends C15ProxyIface {
    constructor() {
        super();

        this.currentMemoryUsage = 123456;
        this.maxMemoryUsage = 1234567;

        setInterval(() => this.playProgress(), 85);
    }

    getBars(): Bars {
        return this.updateStream.bars;
    }

    buildTime(serverTime: number | undefined): string {
        if (!serverTime)
            return "";

        var t = serverTime - this.updateStream.timingInfo.serverTime + this.updateStream.timingInfo.localTime;
        var d = new Date(t / 1000 / 1000);
        return d.getHours().toString().padStart(2, '0') + ":" + d.getMinutes().toString().padStart(2, '0') + ":" + d.getSeconds().toString().padStart(2, '0');
    }

    setPlaybackPosition(playPos: number): void {
        this.currentPlayPosition = playPos;
        this.synced();
    }

    reset(): void {
        this.updateStream.stop();
        this.updateStream = new MockUpdateStream(this, false);
    }

    toggleRecording(): void {
        this.recordPaused = !this.recordPaused;
        this.synced();
    }

    pausePlayback(): void {
        this.playbackPaused = true;
        this.synced();
    }

    startPlayback(): void {
        this.playbackPaused = false;
        this.synced();
    }

    playProgress() {
        if (!this.playbackPaused) {
            this.currentPlayPosition++;
            this.synced();
        }
    }

    undo(): void {
    }

    download(from: number, to: number): void {
    }

    private updateStream = new MockUpdateStream(this, true);
}

class C15Proxy extends C15ProxyIface {
    buildTime(serverTime: number | undefined): string {
        if (!serverTime)
            return "";

        var t = serverTime - this.updateStream.timingInfo.serverTime + this.updateStream.timingInfo.localTime;
        var d = new Date(t / 1000 / 1000);
        return d.getHours().toString().padStart(2, '0') + ":" + d.getMinutes().toString().padStart(2, '0') + ":" + d.getSeconds().toString().padStart(2, '0');
    }

    getBars(): Bars {
        return this.updateStream.bars;
    }

    setPlaybackPosition(playPos: number): void {
        this.fireAndForget({ "set-playback-position": { "frameId": Math.max(playPos, 0) } });
    }

    pausePlayback(): void {
        this.fireAndForget({ "pause-playback": {} });
    }

    startPlayback(): void {
        this.fireAndForget({ "start-playback": {} });
    }

    reset() {
        this.fireAndForget({ "reset": {} });
    }

    toggleRecording() {
        this.fireAndForget({ "toggle-recording": {} });
    }

    undo() {
        var bar = this.getBars().get(this.getCurrentPlayPosition());
        if (bar) {
            var r = new XMLHttpRequest();
            var url = "http://" + hostName + playgroundHttpPort + "/presets/param-editor/restore?timestamp=" + bar.recordTime;
            r.open("GET", url, true);
            r.send();
        }
    }

    download(from: number, to: number) {
        var url = "http://" + hostName + httpPort + "/?begin=" + from + "&end=" + to;
        window.location.assign(url);
    }

    private fireAndForget(msg: Object) {
        var webSocket = new WebSocket("ws://" + hostName + wsPort);

        webSocket.onopen = function (event) {
            webSocket.send(JSON.stringify(msg));
        };
    }

    private updateStream = new UpdateStream(this);

}