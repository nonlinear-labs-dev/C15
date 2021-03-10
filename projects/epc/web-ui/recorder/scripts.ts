const httpPort = ":8890";
const wsPort = ":8889";
const hostName = location.hostname.length == 0 ? "localhost" : location.hostname;

window.onload = function () {
    var mock = location.search.includes("mock-audio-data");
    var updateStream = mock ? new MockUpdateStream() : new UpdateStream();
    var display = new UI(updateStream.bars, updateStream.timingInfo);
    updateStream.connect(display);
}
