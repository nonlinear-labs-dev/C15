var InterpolationStepSize = 50;

function guid() {
  function s4() {
    return Math.floor((1 + Math.random()) * 0x10000)
      .toString(16)
      .substring(1);
  }
  return s4() + s4() + '-' + s4() + '-' + s4() + '-' + s4() + '-' + s4() + s4() + s4();
};

function MacroIDToIndex(mcID) {
  switch(Number(mcID)) {
    case 243:
      return 0;
    case 244:
      return 1;
    case 245:
      return 2;
    case 246:
      return 3;
    case 369:
      return 4;
    case 371:
      return 5;
  }

  return -1;
}

class UUID {
  constructor() {
    this.uuid = guid();
  }
}

class Slot {
  constructor(id) {
    this.paramID = id;
    this.callbacks = [];
  }
  connect(cb) {
    this.callbacks.push(cb);
  }
  onChange(val) {
    for(var i = 0; i < this.callbacks.length; i++) {
      var cb = this.callbacks[i];
      cb(val, this.paramID);
    }
  }
}

class ServerProxy {
  constructor(onStartCB) {
    this.onStartCB = onStartCB;
    this.uuid = new UUID();
    this.scheduleReconnection();
  }

  scheduleReconnection(timeOut) {
    let that = this;
    var connectFunc = function() {
      that.webSocket = null;
      //that.webSocket = new WebSocket("ws://localhost:8080/ws-mc/"); //Local
      //that.webSocket = new WebSocket("ws://192.168.0.2:8080/ws-mc/"); //Buildserver
      that.webSocket = new WebSocket('ws://192.168.8.2:80/ws-mc/'); //Production
      that.webSocket.onopen = that.onStartCB;
      that.webSocket.onmessage = that.onMessage;

      that.webSocket.onclose = function(event) {
        let connectionDeadElement = document.getElementById('connection-dead');
        connectionDeadElement.style.display = 'block';
        that.scheduleReconnection(2500);
      };
    }
    
    if(timeOut) {
      setTimeout(connectFunc, timeOut);
    } else {
      connectFunc();
    }
  }

  getValueForKeyFromMessage(message, key) {
    var ret = null;
    if(message.includes(key)) {
      var keyPos = message.indexOf(key + "=");
      var endPos = message.indexOf("&", keyPos);
      if(endPos !== -1) {
          ret = message.slice(keyPos + key.length + 1, endPos);
  	} else {
      	ret = message.slice(keyPos + key.length + 1);
      }
    }
    return ret;
  }

  onMessage(event) {
    var message = event.data;
    if(message.startsWith("MCVIEW")) {

      var id = serverProxy.getValueForKeyFromMessage(message, "ID");
      var val = Number(Number(serverProxy.getValueForKeyFromMessage(message, "VAL")) * 100).toFixed(3);
      var uuid = serverProxy.getValueForKeyFromMessage(message, "UUID");
      var name = serverProxy.getValueForKeyFromMessage(message, "NAME");

      var paramNumber = id.split("-")[1];

      var mc = model.mcs[MacroIDToIndex(paramNumber)];
      if(mc !== undefined) {
        mc.setName(name);
        if(uuid !== serverProxy.uuid.uuid || uuid === "FORCE") {
          mc.active = false;
          mc.setValue(val);
        } else {
          mc.active = true;
        }
      }
    }
  }

  send(path, message) {
    this.webSocket.send(path + message + "&uuid=" + this.uuid.uuid);
  }
}

class Timer {
  constructor(length) {
    this.length = length;
    this.start = Date.now();
  }
  expired() {
    return Date.now() >= this.start + this.length;
  }
  restart() {
    this.start = Date.now();
  }
}

class MC {
  constructor(mcID) {
    this.paramID = mcID;
    this.paramValue = 0;
    this.targetValue = undefined;
    this.onValueChanged = new Slot(mcID);
    this.onTargetChanged = new Slot(mcID);
    this.changed = new Slot(mcID);
    this.throttler = new Timer(1);
    this.updateThrottler = new Timer(20);
    this.callBackAfterUpdate = undefined;
    this.active = false;
    this.givenName = "";
  }

  setCallbackAfterUpdate(cb) {
    this.callBackAfterUpdate = cb;
  }

  setTarget(val) {
    if(serverProxy.webSocket.readyState == 1) {
      this.targetValue = val;
      this.onTargetChanged.onChange(val);
    }
  }

  updateValue(val) {
    var old = this.paramValue;
    if(old !== val) {
      this.paramValue = val;
      this.changed.onChange();

      if(this.throttler.expired()) {
        this.sendMC();
        this.throttler.restart();
      }
    }
  }

  setValue(val) {
    this.paramValue = val;
    this.targetValue = undefined;
    this.onValueChanged.onChange(val);
  }

  setName(name) {
    this.givenName = name;
  }

  connectValue(cb) {
    this.onValueChanged.connect(cb);
  }

  sendMC() {
    var scaled = this.paramValue.toFixed(3) / 100;
    serverProxy.send("/set-mc", "?id="+this.paramID+"&value="+scaled);
  }

  update() {
    var step = 1.0 / ((InterpolationStepSize) * 0.5 + 1.0);
		var foo = (1 - step) * this.paramValue + step * this.targetValue;
    this.updateValue(foo);


    if(this.callBackAfterUpdate) {
      this.callBackAfterUpdate(this);
    }
  }
}

class MCModel {
  constructor() {
    this.mcs = [];
    this.mcs[0] = new MC(243);
    this.mcs[1] = new MC(244);
    this.mcs[2] = new MC(245);
    this.mcs[3] = new MC(246);
    this.mcs[4] = new MC(369);
    this.mcs[5] = new MC(371);

    setInterval(this.update.bind(this), 20);
  }

  setTarget(id, target) {
    model.mcs[MacroIDToIndex(id)].setTarget(target);
  }

  update() {
    var i;
    for(i = 0; i < model.mcs.length; i++) {
      var mc = model.mcs[i];
      if(mc.targetValue !== undefined && mc.targetValue !== mc.paramValue) {
        mc.update();
      }
    }
  }
}

class RangeDivision {
  constructor() {
    this.controls = [{"ID":0,"x":0,   "y":0,    "w":0.5,  "h":0.62,  "type":"xy",  "MCX":243,  "MCY":244},
                     {"ID":1,"x":0.5, "y":0,    "w":0.5,  "h":0.62,  "type":"xy",  "MCX":369, "MCY":371},
                     {"ID":2,"x":0,   "y":0.62,  "w":1,    "h":0.19, "type":"x",   "MCX":245,  "MCY":null},
                     {"ID":3,"x":0,   "y":0.81, "w":1,    "h":0.19, "type":"x",   "MCX":246,  "MCY":null}];

    this.deadzones = [{"ID":0,"x":0.025,"y":0.05},
                      {"ID":1,"x":0.025,"y":0.05},
                      {"ID":2,"x":0.025,"y":0.05},
                      {"ID":3,"x":0.025,"y":0.05}];

    this.leftMargin = 0.015;


    try {
      var val = window.localStorage.getItem("xy");
      if(val !== null) {
        if(val === "true") {
          setCDXY(this);
          document.getElementById("cd-x-f").checked = false;
          document.getElementById("cd-xy-f").checked = true;
        } else {
          setCDX(this);
          document.getElementById("cd-xy-f").checked = false;
          document.getElementById("cd-x-f").checked = true;
        }
      }
    } catch(err) {
      setCDX(this);
      document.getElementById("cd-x-f").checked = true;
      document.getElementById("cd-xy-f").checked = false;
      console.log(err);
    }
  }
}

class ColorScheme {
  constructor() {
    this.markerColor = "rgb(130, 130, 130)";
    this.labelColor = "rgb(130, 130, 130)";
    this.blueIndicator = "rgb(50,140,255)";
    this.grayIndicator = "rgb(156,156,156)";
    this.settingsHandle = "rgb(180,180,180)";
    this.backgroundColor = "rgb(30,30,30)";
    this.playZoneColor = "#2b2b2b";
  }
}

var fiveMM = 5

var mmToPx = function(mm) {
    var div = document.createElement('div');
    div.style.display = 'block';
    div.style.height = '100mm';
    document.body.appendChild(div);
    var convert = div.offsetHeight * mm / 100;
    div.parentNode.removeChild(div);
    return convert;
};

var observer = new MutationObserver(function() {
                  if (document.body) {
                    fiveMM = mmToPx(5);
                    observer.disconnect();
                  }
                });
                observer.observe(document.documentElement, {childList: true});





class MCView {
  constructor() {
    this.canvas = document.getElementById('canvas');
    this.range = new RangeDivision();

    model.mcs.forEach(function(mc) {
      mc.onValueChanged.connect(function(val, id) {
        view.redraw(model);
      });

      mc.changed.connect(function(val, id) {
        view.redraw(model);
      });
    });

    this.imgageloaded = false;
    this.fontsloaded = false;

    this.img = new Image();
    this.img.onload = () => {
      this.imgageloaded = true;

      if(this.fontsloaded)
        view.redraw(model);
    }

    document.fonts.onloadingdone = (fontFaceSetEvent) => {
      this.fontsloaded = true;

      if(this.imgageloaded)
        view.redraw(model);
    };

    this.img.src = "tri-open.svg";

    this.body = document.getElementById('body');
    this.addEventsToElement(this.canvas);
  }


  getMCForPagePos(pageX, pageY) {
    var canvasWidth = view.canvas.width;
    var leftMargin = view.range.leftMargin * canvasWidth;
    leftMargin = Math.max(leftMargin, fiveMM);
    var width = canvasWidth - leftMargin;
    var heigth = view.canvas.height;
    var mc = null;
    var i;
    for(i in view.range.controls) {
      var division = view.range.controls[i];
      var deadZones = view.range.deadzones[i];

      var x = leftMargin + width * division.x;
      var y = heigth * division.y;
      var w = width * division.w;
      var h = heigth * division.h;

      var wD = w - width * deadZones.x;
      var hD = h - heigth * deadZones.y;
      var xD = x + (w - wD) / 2;
      var yD = y + (h  - hD) / 2;


      if(pageX >= xD && pageX <= xD + wD  &&
         pageY >= yD && pageY <= yD + hD) {
        mc = division.MCX;
      }
    }
    return mc;
  }

  getMCForTouch(touch) {
    return this.getMCForPagePos(touch.pageX, touch.pageY);
  }

  addEventsToElement(element) {
    element.addEventListener('touchstart', function(event) {
      var found = false;
      var newTouches = [];

      for(var i = 0; i < event.changedTouches.length; i++) {
        var changedTouch = event.changedTouches[i];
        controller.touches.forEach(function(touch) {
          if(touch.touch.identifier === changedTouch.identifier) {
            found = true;
          } else {
            newTouches.push(changedTouch);
          }
        });
      }

      var currTouch = event.changedTouches[0];
      if(currTouch) {
        var found = controller.touches.some(function(touch) {
          return touch.touch.identifier === currTouch.identifier;
        });

        if(!found) {
          var mc = view.getMCForTouch(currTouch);
          if(mc !== null) {
            controller.touches.push({"touch":currTouch, "mc":mc});
            controller.onChange();
          }
        }
      }
    });

    element.addEventListener('mousemove', function(event) {
      controller.lastMouseEvent = event;
      controller.onChange();
    });

    element.addEventListener('mousedown', function() {
       controller.mouseDown = 1;
       controller.onChange();
    });

    element.addEventListener('mouseup', function() {
       controller.mouseDown = 0;
       controller.lastMouseMC = null;
       controller.onChange();
    });

    element.addEventListener('touchmove', function(event) {
      var changed = false;

      event.preventDefault();
      controller.touches.forEach(function(inputTouch) {
        var touches = event.touches;
        var l = touches.length;
        for(var i = 0; i < l; i++) {
          var eventTouch = touches[i];
          if(inputTouch.touch.identifier === eventTouch.identifier) {
            inputTouch.touch = eventTouch;
            changed = true;
          }
        }
      });

      if(changed)
        controller.onChange();
    });

    element.addEventListener('touchend', function(event) {

      var endingTouch = event.changedTouches[0];

      if(endingTouch) {
        var filtered = controller.touches.filter(function(touch){
            return touch.touch.identifier !== endingTouch.identifier;
        });

        controller.touches = filtered;
        controller.onChange();
      }

    });

    element.addEventListener('click', function(event) {
      if(document.getElementById('settings-overlay').classList.contains("collapsed")) {
        var canvas = view.canvas;
        var middle = canvas.height / 2;
        var height = (canvas.height / 100) * 2.5;
        var leftMargin = canvas.width * view.range.leftMargin;
        leftMargin = Math.max(leftMargin, fiveMM);
        var width = leftMargin * 0.6;

        if(event.pageX <= leftMargin) {
          toggleSettings();
        }
      }
    });
  }

  redraw(model) {
    var canvas = view.canvas;
    var ctx = canvas.getContext('2d');
    var info = view.getViewInfo();
    canvas.width  = window.innerWidth;
    canvas.height = window.innerHeight;
    var canvasWidth = canvas.width;
    var leftMargin = canvasWidth * view.range.leftMargin;
    leftMargin = Math.max(leftMargin, fiveMM);
    document.getElementById("settings-opener").style.width = leftMargin + "px";
    var width = canvasWidth - leftMargin;
    var heigth = canvas.height;

    ctx.font = '20px CanvasFont';
    ctx.strokeStyle = "black";
    ctx.fillStyle = new ColorScheme().backgroundColor;
    ctx.fillRect(0, 0, canvasWidth, heigth);

    var i;
    for(i in info.controls) {
      var division = info.controls[i];
      var deadZones = view.range.deadzones[i];
      var x = leftMargin + width * division.x;
      var y = heigth * division.y;
      var w = width * division.w;
      var h = heigth * division.h;

      ctx.beginPath();
      ctx.lineWidth = "1";
      ctx.strokeStyle = "black";

      ctx.rect(x, y, w, h);
      ctx.stroke();

      ctx.beginPath();
      ctx.fillStyle = new ColorScheme().playZoneColor;
      var wD = w - width * deadZones.x;
      var hD = h - heigth * deadZones.y;
      var xD = x + (w - wD) / 2;
      var yD = y + (h  - hD) / 2;

      ctx.fillRect(xD, yD, wD, hD);
      ctx.fill();
      this.drawHandle(division, xD, yD, wD, hD);
    }
  }

  drawHandle(division, xD, yD, wD, hD) {
    if(division.type == null)
      return;
    if(division.type.startsWith("xy")) {
      this.draw2DDivision(division, xD, yD, wD, hD);
    } else if(division.type.startsWith("x")) {
      this.draw1DDivision(division, xD, yD, wD, hD);
    }
  }

  draw2DDivision(division, xD, yD, wD, hD) {
    var canvas = view.canvas;
    var ctx = canvas.getContext('2d');
    var x = xD;
    var y = yD;
    var w = wD;
    var h = hD;

    var xTarget = model.mcs[MacroIDToIndex(division.MCX)].targetValue;
    var yTarget = model.mcs[MacroIDToIndex(division.MCY)].targetValue;
    var xVal = model.mcs[MacroIDToIndex(division.MCX)].paramValue;
    var yVal = model.mcs[MacroIDToIndex(division.MCY)].paramValue;
    var yLabel = getUnicodeForMC(division.MCY);
    var yName =  model.mcs[MacroIDToIndex(division.MCY)].givenName;
    var xLabel = getUnicodeForMC(division.MCX);
    var xName = model.mcs[MacroIDToIndex(division.MCX)].givenName;

    var size = canvas.width / 100;

    ctx.strokeStyle = new ColorScheme().markerColor;
    ctx.fillStyle = new ColorScheme().labelColor;
    ctx.font = '20px CanvasFont_UNICODE';
    var lineHeight=ctx.measureText("\uE101").width;

    var offset = lineHeight;

    var upperLeftFixPointX = x + offset;
    var upperLeftFixPointY = y + offset;
    var lowerRightFixPointX = x + w - offset;
    var lowerRightFixPointY = y + h - offset * 1.3;

    var getTextWidth = function(text) {
      return view.canvas.getContext("2d").measureText(text).width;
    };

    //X
    ctx.fillText(xLabel, lowerRightFixPointX - getTextWidth(xLabel) / 2, lowerRightFixPointY);
    ctx.font = '20px CanvasFont';
    ctx.fillStyle = new ColorScheme().markerColor;
    ctx.fillText(Number(xVal).toFixed(1), lowerRightFixPointX - getTextWidth(Number(xVal).toFixed(1)) / 2, lowerRightFixPointY + lineHeight * 1.05);
    ctx.fillText(xName, lowerRightFixPointX - getTextWidth(xName) - getTextWidth(xLabel), lowerRightFixPointY);
    //Y
    ctx.fillStyle = new ColorScheme().labelColor;
    ctx.font = '20px CanvasFont_UNICODE';
    ctx.fillText(yLabel, upperLeftFixPointX - getTextWidth(yLabel) / 2, upperLeftFixPointY);
    ctx.font = '20px CanvasFont';
    ctx.fillStyle = new ColorScheme().markerColor;
    ctx.fillText(Number(yVal).toFixed(1), upperLeftFixPointX - getTextWidth(Number(yVal).toFixed(1)) / 2, upperLeftFixPointY + lineHeight * 1.05);
    ctx.fillText(yName, upperLeftFixPointX + getTextWidth(yLabel), upperLeftFixPointY);

    ctx.beginPath();
    ctx.strokeStyle = "transparent";
    ctx.fillStyle = new ColorScheme().blueIndicator;

    yTarget = 100 - yTarget;
    yVal = 100 - yVal;

    size = Math.max(size, 12);

    ctx.arc(x + w / 100 * xVal, y + h / 100 * yVal, size, 0, 2*Math.PI, true);
    ctx.stroke();
    ctx.fill();

    if(xTarget !== undefined && yTarget !== undefined && xTarget !== xVal || yTarget !== yVal) {
      ctx.beginPath();
      ctx.lineWidth = "3";
      ctx.strokeStyle = new ColorScheme().grayIndicator;
      ctx.fillStyle = "transparent";
      ctx.arc(x + w / 100 * xTarget, y + h / 100 * yTarget, size, 0, 2*Math.PI, true);
      ctx.fill();
      ctx.stroke();
    }
  }

  draw1DDivision(division, xD, yD, wD, hD) {
    var canvas = view.canvas;
    var ctx = canvas.getContext('2d');
    var x = xD;
    var y = yD;
    var w = wD;
    var h = hD;

    var xVal = model.mcs[MacroIDToIndex(division.MCX)].paramValue;
    var xTarget = model.mcs[MacroIDToIndex(division.MCX)].targetValue;
    var xLabel = getUnicodeForMC(division.MCX);
    var xName = model.mcs[MacroIDToIndex(division.MCX)].givenName;

    ctx.beginPath();
    ctx.strokeStyle = new ColorScheme().markerColor;
    ctx.fillStyle = new ColorScheme().labelColor;
    ctx.font = '20px CanvasFont_UNICODE';

    var lineHeight=ctx.measureText("\uE101").width;
    var offset = lineHeight;
    var lowerRightFixPointX = x + w - offset;
    var lowerRightFixPointY = y + h - offset * 1.3;

    var getTextWidth = function(text) {
      return view.canvas.getContext("2d").measureText(text).width;
    };

    ctx.fillText(xLabel, lowerRightFixPointX - getTextWidth(xLabel) / 2, lowerRightFixPointY);
    ctx.font = '20px CanvasFont';
    ctx.fillStyle = new ColorScheme().markerColor;
    ctx.fillText(Number(xVal).toFixed(1), lowerRightFixPointX - getTextWidth(Number(xVal).toFixed(1)) / 2, lowerRightFixPointY + lineHeight * 1.05);
    ctx.fillText(xName, lowerRightFixPointX - getTextWidth(xName) - getTextWidth(xLabel), lowerRightFixPointY);

    if(xTarget !== undefined && xTarget !== xVal) {
      ctx.beginPath();
      ctx.lineWidth = Math.max(w / 200, 8);
      ctx.fillStyle = "transparent";
      ctx.strokeStyle = new ColorScheme().grayIndicator;
      ctx.moveTo(xD + wD / 100 * xTarget, yD + 1);
      ctx.lineTo(xD + wD / 100 * xTarget, yD + hD - 1);
      ctx.stroke();
      ctx.fill();
    }

    ctx.beginPath();
    ctx.fillStyle = "transparent";
    ctx.strokeStyle = new ColorScheme().blueIndicator;
    ctx.lineWidth = Math.max(w / 200, 8);
    ctx.moveTo(x + w / 100 * xVal, y + 1);
    ctx.lineTo(x + w / 100 * xVal, y + h - 1);
    ctx.stroke();
    ctx.fill();
  }

  getViewInfo() {
    var canvas = document.getElementById('canvas');
    return {"canvas-w": canvas.width, "canvas-h": canvas.height, "controls": this.range.controls}
  }
}

function getUnicodeForMC(mcId) {
	switch(mcId) {
		case 243:
		return "\uE100";
		case 244:
		return "\uE101";
		case 245:
		return "\uE102";
		case 246:
		return "\uE103";
		case 369:
		return "\uE104";
		case 371:
		return "\uE105";
	}
}

class Input {
  constructor(x, y, originMC) {
    this.x = x;
    this.y = y;
    this.mc = originMC;
  }
}

class MCController {
  constructor() {
    this.touches = [];
    this.lastMouseEvent = null;
    this.mouseDown = 0;
    this.userInputs = null;
    this.lastMouseMC = null;
  }

  onChange() {
    this.userInputs = this.collectInputs();
    this.inputsToTargets();
  }

  inputsToTargets() {
    var targets = [];
    var displayInformation = view.getViewInfo();

    var i;
    for(i in displayInformation.controls) {
      var division = displayInformation.controls[i];
      var deadzone = view.range.deadzones[i];
      var activeInputs = [];
      var canvasWidth = view.canvas.width;
      var leftMargin = canvasWidth * view.range.leftMargin;
      leftMargin = Math.max(leftMargin, fiveMM);
      var cW = canvasWidth - leftMargin;
      var cH = view.canvas.height;
      var dX = leftMargin + (cW * division.x);
      var dY = (cH * division.y);
      var dW = cW * division.w;
      var dH = cH * division.h;

      var ctx = view.canvas.getContext("2d");

      var wD = dW - cW * deadzone.x;
      var hD = dH - cH * deadzone.y;
      var xD = dX + (dW - wD) / 2;
      var yD = dY + (dH - hD) / 2;

      var padding = 2;

      for(var iI in this.userInputs) {
        var input = this.userInputs[iI];

        if(input.mc !== division.MCX && input.mc !== division.MCY)
          continue;


        var cappedX = Math.max(Math.min(input.x, xD + wD + padding), xD - padding);
        var cappedY = Math.max(Math.min(input.y, yD + hD + padding), yD - padding);

        input.x = cappedX;
        input.y = cappedY;

        if(input.mc !== null) {
          activeInputs.push(input);
        }
      }

      var inputCount = activeInputs.length;
      if(inputCount <= 0 || division.type == null) {
        continue;
      }

      var x = (wD - dW) / 2;
      var y = (hD - dH) / 2;

      activeInputs.forEach(function(input) {
        x+=input.x;
        y+=input.y;
      });

      //HUH?! TODO: Investigate
      if(division.type.startsWith("xy") && (division.MCX === 369 || division.MCX === 245)) {
        x -= xD;
        x += (dW - wD) / 2;
      } else {
        x -= leftMargin;
      }

      if(inputCount > 0) {
        x /= inputCount;
        y /= inputCount;
      }

      var xVal = Number(x / wD * 100).toFixed(1);
      var yVal = 100 - Number(y / hD * 100).toFixed(1);

      //Just To Be Safe!
      xVal = Math.min(100, Math.max(xVal, 0));
      yVal = Math.min(100, Math.max(yVal, 0));

      if(division.type.startsWith("xy") && division.MCX !== null && division.MCY !== null) {
        if(  xVal !== model.mcs[MacroIDToIndex(division.MCX)].targetValue ||
          yVal !== model.mcs[MacroIDToIndex(division.MCY)].targetValue) {
          model.setTarget(division.MCX, xVal);
          model.setTarget(division.MCY, yVal);
        }
      } else if(division.type.startsWith("x") && division.MCX) {
        if(xVal !== model.mcs[MacroIDToIndex(division.MCX)].targetValue) {
          model.setTarget(division.MCX, xVal);
        }
      }
    }
  }

  collectInputs() {
    var activePositions = [];

    this.touches.forEach(function(touch) {
      activePositions.push(new Input(touch.touch.pageX, touch.touch.pageY, touch.mc));
    });

    if(this.lastMouseEvent !== undefined) {
      if(this.mouseDown !== 0) {
        if(this.lastMouseMC === null) {
          this.lastMouseMC = view.getMCForPagePos(this.lastMouseEvent.pageX, this.lastMouseEvent.pageY);
        }

        activePositions.push(new Input(this.lastMouseEvent.pageX, this.lastMouseEvent.pageY, this.lastMouseMC));
      }
    }

    return activePositions;
  }
}

var webSocket;
var model;
var view;
var controller;
var serverProxy;

function setCDX(range) {
   range.controls[1]["MCX"] = 369;
   range.controls[1]["MCY"] = 371;
   range.controls[2]["MCX"] = 245;
   range.controls[2]["MCY"] = null;
   range.controls[3]["MCX"] = 246;
   range.controls[3]["MCY"] = null;

   try {
     window.localStorage.setItem("xy", false);
   } catch(err) {
     console.log(err);
   }
}

function setCDXY(range) {
  range.controls[1]["MCX"] = 245;
  range.controls[1]["MCY"] = 246;
  range.controls[2]["MCX"] = 369;
  range.controls[2]["MCY"] = null;
  range.controls[3]["MCX"] = 371;
  range.controls[3]["MCY"] = null;

  try {
    window.localStorage.setItem("xy", true);
  } catch(err) {
    console.log(err);
  }
}

function onLoad() {
  var href = window.location.href;
  if(href.includes("settings")) {
    toggleSettings();
  }

  //handle Settings Init and Events
  try {
    var val = window.localStorage.getItem("smoothing");
    if(val !== null) {
      setInterpolation(val);
      document.getElementById("interpolation-step").value = val;
    }
  } catch(err) {
    setInterpolation(50);
    document.getElementById("interpolation-step").value = 50;
    console.log(err);
  }

  document.onkeypress = function(event) {
    if(event.key == "H" || event.key == "h") {
      toggleSettings();
    }
  }

  var toggleXY = () => {
    var xyBox = document.getElementById("cd-xy-f");
    var xBox = document.getElementById("cd-x-f");

    xBox.checked = xyBox.checked;
    xyBox.checked = !xBox.checked;

    if(xBox.checked) {
      setCDX(view.range);
    } else {
      setCDXY(view.range);
    }

    view.redraw(model);
  };

  document.getElementById("cd-xy").onclick = () => {
    toggleXY()
  };

  document.getElementById("cd-x").onclick = () => {
    toggleXY();
  };
  //End Events for Settings

  serverProxy = new ServerProxy(function() {
    model = new MCModel(serverProxy.webSocket);
    view = new MCView();
    controller = new MCController();
    let connectionDeadElement = document.getElementById('connection-dead');
    connectionDeadElement.style.display = 'none';

    setInterval(function() {
      var changed = false;
      model.mcs.forEach(function(mc){
        if(mc !== undefined && mc.targetValue !== undefined && mc.paramValue !== undefined) {
          if(Number(mc.paramValue).toFixed(1) === Number(mc.targetValue).toFixed(1)) {
            mc.targetValue = undefined;
            changed = true;
          }
        }
      });
      if(changed)
        view.redraw(model);
    }, 50);
  });
}

//UI Functionality
function toggleSettings() {
  var e = document.getElementById("settings-overlay");
  e.classList.toggle("collapsed");
  e.classList.toggle("full");

  var op = document.getElementById("settings-opener");
  op.classList.toggle("collapsed");
}

function setInterpolation(val) {
  InterpolationStepSize = val;
  document.getElementById("interpolation-step-label").innerHTML = val;
  try {
    window.localStorage.setItem("smoothing", val);
  } catch(err) {
    console.log(err);
  }
}

function stub() {

}

function doToggle() {
  if (document.fullscreenElement) {
		document.exitFullscreen();
	} else {
		document.documentElement.requestFullscreen();
	}
}

document.addEventListener("fullscreenchange", () => {
  if(document.fullscreenElement) {
    document.getElementById("fs-select").checked = true;
  } else {
    document.getElementById("fs-select").checked = false;
  }
  setTimeout(() => {
    view.redraw(model);
  }, 3);
});

window.addEventListener('resize', () => {
  view.redraw(model);
});

onLoad();