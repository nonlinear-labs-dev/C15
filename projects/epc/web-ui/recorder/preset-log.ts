/// <reference path="c15-proxy.ts"/>
/// <reference path="scripts.ts"/>

class PresetLogEntry {
    constructor(time: number, info: string) {
        this.info = info;
        this.time = time;
    }

    time: number;
    info: string;
}


class PresetLogStream {
    constructor(private c15: C15ProxyIface) {
        this.connect();
    }

    private connect() {
        console.log("connect preset stream");
        this.log = new Array<PresetLogEntry>();
        this.socket = new WebSocket("ws://" + hostName + wsPresetLogPort);
        this.socket.onopen = (event) => this.getAll();
        this.socket.onerror = (event) => this.retry();
        this.socket.onclose = (event) => this.retry();
        this.socket.onmessage = (event) => this.readMessage(event.data);
    }

    private getAll() {
        this.socket!.send(JSON.stringify({ "getAll": {} }));
    }

    stop() {
        this.close = true;
        if (this.socket)
            this.socket.close();
    }

    find(fromTime: number, toTime: number): PresetLogEntry | null {
        return this.binarySearch(0, this.log.length, fromTime, toTime);
    }

    private binarySearch(from: number, to: number, fromTime: number, toTime: number): PresetLogEntry | null {
        if (to - from < 4) {
            for (var i = from; i < to; i++) {
                if (this.log[i].time >= fromTime && this.log[i].time <= toTime)
                    return this.log[i];
            }
            return null;
        }

        var mid = Math.floor((from + to) / 2);

        if (this.log[mid].time > toTime)
            return this.binarySearch(from, mid, fromTime, toTime);

        if (this.log[mid].time < fromTime)
            return this.binarySearch(mid + 1, to, fromTime, toTime);

        return this.log[mid];
    }

    private retry(): void {
        if (this.retryTimer == -1 && !this.close) {
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
            var json = JSON.parse(reader.result as string);
            if (json[0] == "all") {
                this.log = new Array<PresetLogEntry>();
                for (var a of json[1]) {
                    this.log.push(new PresetLogEntry(a.time, a.info));
                }
            }
            else if (json[0] == "incrementalUpdate") {
                this.log.push(new PresetLogEntry(json[1].time, json[1].info));
            }


            this.c15.synced();
        }

        reader.readAsText(data);
    }


    private socket: WebSocket | null = null;
    private retryTimer = -1;
    private close = false;
    public log = new Array<PresetLogEntry>();
}
