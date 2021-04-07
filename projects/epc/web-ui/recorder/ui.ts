/// <reference path="draggable.ts"/>

function buildTime(serverTime: number, timingInfo: TimingInfo): string {
    var t = serverTime - timingInfo.serverTime + timingInfo.localTime;
    var d = new Date(t / 1000 / 1000);
    return d.getHours().toString().padStart(2, '0') + ":" + d.getMinutes().toString().padStart(2, '0') + ":" + d.getSeconds().toString().padStart(2, '0');
}

function fireAndForget(msg: Object) {
    var webSocket = new WebSocket("ws://" + hostName + wsPort);

    webSocket.onopen = function (event) {
        webSocket.send(JSON.stringify(msg));
    };
}


class UI {
    constructor(bars: Bars, private timingInfo: TimingInfo) {
        this.waveform = new Waveform(bars, timingInfo);
        document.getElementById("toggle-recording")!.onclick = (e) => this.toggleRecording();
        document.getElementById("toggle-playback")!.onpointerdown = (e) => this.togglePlayback(e);
        document.getElementById("reset")!.onclick = (e) => this.reset();
        document.getElementById("waveform")!.onkeydown = (e) => this.togglePlayback(e);
    }

    private togglePlayback(e: Event) {
        var pb = document.getElementById("toggle-playback")!;

        if (pb.classList.contains("paused"))
            fireAndForget({ "start-playback": {} });
        else
            fireAndForget({ "pause-playback": {} });

        e.stopPropagation();
        e.preventDefault();
    }

    updateFirstAndLastFrame(firstTime: number, lastTime: number) {
        document.getElementById("first-available-frame-timestamp")!.textContent = buildTime(firstTime, this.timingInfo);
        document.getElementById("last-available-frame-timestamp")!.textContent = buildTime(lastTime, this.timingInfo);
    }

    updateMemoryUsage(current: number, max: number) {
        document.getElementById("consumed-mem")!.textContent = this.formatMemSize(current);
        document.getElementById("max-mem")!.textContent = this.formatMemSize(max);
    }

    updateTransportStates(recorderPaused: boolean, playPaused: boolean, playPos: number) {
        var rec = document.getElementById("toggle-recording")!;
        var pb = document.getElementById("toggle-playback")!;

        if (this.waveform.playPos > 0)
            pb.classList.remove("disabled");
        else
            pb.classList.add("disabled");

        this.waveform.setPlayPos(playPos);

        if (recorderPaused) {
            rec.classList.remove("recording");
            rec.classList.add("paused");
        }
        else {
            rec.classList.add("recording");
            rec.classList.remove("paused");
        }

        if (playPaused) {
            pb.classList.remove("playing");
            pb.classList.add("paused");
        }
        else {
            pb.classList.add("playing");
            pb.classList.remove("paused");
        }
    }

    update() {
        this.waveform.update();
    }

    private formatMemSize(s: number) {
        var i = 0;
        var units = [' B', ' kB', ' MB'];

        while (s > 1024 && i < 2) {
            s = s / 1024;
            i++;
        }

        return Math.max(s, 0.1).toFixed(1) + units[i];
    }

    private reset() {
        fireAndForget({ "reset": {} });
    }

    private toggleRecording() {
        fireAndForget({ "toggle-recording": {} });
    }

    private updateStream: UpdateStream | null = null;
    private waveform: Waveform;
}
