class UI {
    constructor(private c15: C15ProxyIface) {
        this.waveform = new Waveform(c15);
        document.getElementById("toggle-recording")!.onclick = (e) => c15.toggleRecording();

        var pb = document.getElementById("toggle-playback")!;
        pb.onclick = (e) => {
            if (pb.classList.contains("disabled"))
                return;

            c15.togglePlayback();
        }

        var undo = document.getElementById("undo")!;
        undo.onclick = (e) => {
            if (undo.classList.contains("disabled"))
                return;

            c15.undo();
        }

        var download = document.getElementById("download")!;
        download.onclick = (e) => {
            if (download.classList.contains("disabled"))
                return;

            e.stopPropagation();
            e.preventDefault();
            this.download();
        }

        document.getElementById("reset")!.onclick = (e) => {
            c15.reset();
            this.waveform.selectedRange.barRange.reset();
        }

        document.onkeypress = (e) => {
            if (document.activeElement && document.activeElement.tagName == "INPUT")
                return;

            var undoEnabled = !undo.classList.contains("disabled");
            var downloadEnabled = !download.classList.contains("disabled");

            if (e.key == ' ')
                c15!.togglePlayback();
            else if ((e.key == 'z' || e.key == 'Z') && undoEnabled)
                c15!.undo();
            else if (e.key == 'r' || e.key == 'R')
                c15!.toggleRecording();
            else if ((e.key == 's' || e.key == 'S') && downloadEnabled)
                this.download();
            else if (e.key == 'ZoomIn' || e.key == '+')
                this.waveform.zoomIn();
            else if (e.key == 'ZoomOut' || e.key == '-')
                this.waveform.zoomOut();

            this.update();
        }

        document.onkeydown = (e) => {
            if (document.activeElement && document.activeElement.tagName == "INPUT")
                return;

            if (e.key == 'ArrowLeft')
                this.waveform.scrollBy(50);
            else if (e.key == 'ArrowRight')
                this.waveform.scrollBy(-50);
            else if (e.key == 'ArrowUp')
                console.log("prev preset");
            else if (e.key == 'ArrowDown')
                console.log("next preset");

            this.update();
        };

        document.getElementById("waveform")!.onkeydown = (e) => c15.togglePlayback();

    }

    download() {
        const begin = this.waveform.selectedRange.barRange.min();
        const end = this.waveform.selectedRange.barRange.max();

        this.c15.prepareDownload(begin, end, (info: PrepareDownloadInfo) => {
            var dialog = document.getElementById("download-dialog")! as HTMLDivElement;
            dialog.classList.remove("hidden");

            const name = document.getElementById("download-name")! as HTMLInputElement;

            const flacSize = document.getElementById("flac-size")! as HTMLSpanElement;
            flacSize.innerText = this.formatMemSize(info.flac.size);

            const waveSize = document.getElementById("wave-size")! as HTMLSpanElement;
            const tooLargeForWave = info.wave.size >= Math.pow(2, 32);
            waveSize.innerText = tooLargeForWave ? "(too large)" : this.formatMemSize(info.wave.size);

            const cancel = document.getElementById("download-cancel")! as HTMLButtonElement;
            cancel.onclick = (e) => {
                this.defaultDownloadOption = cancel.id;
                dialog.classList.add("hidden");
            }

            const flac = document.getElementById("download-flac")! as HTMLButtonElement;
            flac.onclick = (e) => {
                this.defaultDownloadOption = flac.id;
                dialog.classList.add("hidden");
                this.downloadFile(name.value + ".flac", begin, end);
            }

            const wave = document.getElementById("download-wave")! as HTMLButtonElement;
            wave.onclick = (e) => {
                this.defaultDownloadOption = wave.id;
                dialog.classList.add("hidden");
                this.downloadFile(encodeURIComponent(this.sanitizeFileName(name.value) + ".wav"), begin, end);
            }

            wave.disabled = tooLargeForWave;

            name.oninput = () => {
                wave.disabled = name.value.length == 0;
                flac.disabled = name.value.length == 0;
            }

            var presetName = this.c15.getLoadedPresetAt(info.range.from);
            if (!presetName) presetName = "Recording";
            name.value = this.sanitizeFileName("C15 [" + presetName + "] (" + this.toLocaleTime(info.range.from) + " to " + this.toLocaleTime(info.range.to) + ")");

            const resetName = document.getElementById("reset-download-name")! as HTMLAnchorElement;
            resetName.onclick = () => {
                name.value = "";
                wave.disabled = true;
                flac.disabled = true;
            }

            var def = document.getElementById(this.defaultDownloadOption) as HTMLButtonElement;
            def.focus();

        });
    }

    downloadFile(name: string, from: number, to: number) {
        var url = "http://" + hostName + httpPort + "/" + name + "?begin=" + from + "&end=" + to;
        var dlink = document.createElement('a') as HTMLAnchorElement;
        dlink.download = name;
        dlink.href = url;
        dlink.click();
        dlink.remove();
    }

    sanitizeFileName(a: string): string {
        return a.replace(/[/\\?%*:|"<>]/g, '-');
    }

    toLocaleTime(d: number): string {
        return new Date(d / 1000 / 1000).toLocaleString();
    }

    update() {
        if (this.c15.getConnectionState() == ConnectionState.Connected)
            document.getElementById("not-connected-box")!.classList.add("hidden");
        else
            document.getElementById("not-connected-box")!.classList.remove("hidden");

        document.getElementById("first-available-frame-timestamp")!.textContent = this.c15.buildTime(this.c15.getBars().first().recordTime);
        document.getElementById("last-available-frame-timestamp")!.textContent = this.c15.buildTime(this.c15.getBars().last().recordTime);
        document.getElementById("consumed-mem")!.textContent = this.formatMemSize(this.c15.getCurrentMemoryUsage());
        document.getElementById("max-mem")!.textContent = this.formatMemSize(this.c15.getMaxMemoryUsage());
        var rec = document.getElementById("toggle-recording")!;
        var pb = document.getElementById("toggle-playback")!;
        var undo = document.getElementById("undo")!;

        if (this.c15.getCurrentPlayPosition() > 0) {
            pb.classList.remove("disabled");
            undo.classList.remove("disabled");
        }
        else {
            pb.classList.add("disabled");
            undo.classList.add("disabled");
        }

        if (this.c15.getRecordingState() == TransportState.Paused) {
            rec.classList.remove("recording");
            rec.classList.add("paused");
        }
        else {
            rec.classList.add("recording");
            rec.classList.remove("paused");
        }

        if (this.c15.getPlaybackState() == TransportState.Paused) {
            pb.classList.remove("playing");
            pb.classList.add("paused");
        }
        else {
            pb.classList.add("playing");
            pb.classList.remove("paused");
        }

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

    promptOption(prompt: string, cb: VoidFunction) {
        var dia = document.getElementById("prompt-wrapper");
        let diaText: HTMLElement = document.getElementById("prompt-text") as HTMLElement;
        var yes = document.getElementById("prompt-yes");
        var no = document.getElementById("prompt-no");

        diaText!.innerHTML = prompt;

        dia!.classList.remove("hidden");

        var hide = () => {
            dia!.classList.add("hidden");
        };

        yes!.addEventListener("click", (e: Event) => {
            cb();
            hide();
        });

        no!.addEventListener("click", (e: Event) => {
            hide();
        });
    }

    private waveform: Waveform;
    private defaultDownloadOption = "download-cancel";
}
