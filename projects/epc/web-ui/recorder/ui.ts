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
            c15.download(this.waveform.selectedRange.playbackRange.min(), this.waveform.selectedRange.playbackRange.max());
        }

        document.getElementById("reset")!.onclick = (e) => {
            c15.reset();
            this.waveform.selectedRange.playbackRange.reset();

	    }

        document.onkeypress = (e) => {
            var undoEnabled = !undo.classList.contains("disabled");
            var downloadEnabled = !download.classList.contains("disabled");

            if (e.key == ' ')
                c15!.togglePlayback();
            else if ((e.key == 'z' || e.key == 'Z') && undoEnabled)
                c15!.undo();
            else if (e.key == 'r' || e.key == 'R')
                c15!.toggleRecording();
            else if ((e.key == 's' || e.key == 'S') && downloadEnabled)
                c15!.download(this.waveform.selectedRange.playbackRange.min(), this.waveform.selectedRange.playbackRange.max());
            else if (e.key == 'ZoomIn' || e.key == '+')
                this.waveform.zoomIn();
            else if (e.key == 'ZoomOut' || e.key == '-')
                this.waveform.zoomOut();

            this.update();
        }

        document.onkeydown = (e) => {
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

    update() {
        if(this.c15.getConnectionState() == ConnectionState.Connected)
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

    promptOption(prompt : string, cb : VoidFunction) {
        var dia = document.getElementById("prompt-wrapper");
        let diaText : HTMLElement = document.getElementById("prompt-text") as HTMLElement;
        var yes = document.getElementById("prompt-yes");
        var no = document.getElementById("prompt-no");

        diaText!.innerHTML = prompt;

        dia!.classList.remove("hidden");

        var hide = () => {
            dia!.classList.add("hidden");
        };

        yes!.addEventListener("click", (e:Event) => {
            cb();
            hide();
        });

        no!.addEventListener("click", (e:Event) => {            
            hide();
        });
    }

    private waveform: Waveform;
}
