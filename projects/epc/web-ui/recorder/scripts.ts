const hostName = location.hostname.length == 0 ? "localhost" : location.hostname;

window.onload = function () {
    var c15 = C15ProxyIface.create();
    var ui = new UI(c15);
    c15.connect(ui);

    if (location.search.includes("test")) {
        testAddRemove();
        testIntegrals();
    }
}

function testAddRemove() {
    var bars = new Bars(1);
    var b1 = new Bar();
    b1.id = 0;
    b1.max = 0;
    b1.recordTime = 0;
    bars.add(b1);

    var b2 = new Bar();
    b2.id = 9;
    b2.max = 0;
    b2.recordTime = 9;
    bars.add(b2);
    assert(bars.count() == 10);

    var b3 = new Bar();
    b3.id = 10;
    b3.max = 0;
    b3.recordTime = 10;
    bars.add(b3);
    assert(bars.count() == 11);

    var b4 = new Bar();
    b4.id = 11;
    b4.max = 5;
    b4.recordTime = 11;
    bars.add(b4);
    assert(bars.count() == 12);

    bars.remove();
    assert(bars.count() == 11);
    assert(bars.first().recordTime == 1);

    bars.remove();
    assert(bars.count() == 10);
    assert(bars.first().recordTime == 2);

    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();

    assert(bars.count() == 2);
    assert(bars.first().recordTime == 10);

    bars.remove();
    assert(bars.count() == 1);
    assert(bars.first().recordTime == 11);
}

function testIntegrals() {
    var bars = new Bars(1);
    bars.add({ id: 4, max: 10, recordTime: 0 });
    bars.add({ id: 5, max: 10, recordTime: 1 });
    bars.add({ id: 6, max: 10, recordTime: 2 });
    bars.add({ id: 7, max: 17, recordTime: 3 });
    bars.add({ id: 8, max: 1, recordTime: 4 });
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
    bars.remove();
}

