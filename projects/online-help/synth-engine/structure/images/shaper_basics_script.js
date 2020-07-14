// input signal orientation flipped
var shaper = {
	values: {
		drive: 0, drive_f: 0, fold: 0, asym: 0,
	},
	cnv: "", ctx: "",
	init: function() {
		this.cnv = document.getElementById("ShaperPlot");
		window.addEventListener("resize", function(event) {shaper.redraw()});
		this.ctx = this.cnv.getContext("2d");
		this.redraw();
	},
	updateDrive: function(value) {
		this.values.drive = 0.1 * value;
		this.values.drive_f = 0.18 * Math.pow(1.12202, this.values.drive) - 0.18;
		document.getElementById("ShaperDriveValue").innerHTML = Math.floor(this.values.drive) + "." + (Math.floor(10 * this.values.drive) % 10);
		this.redraw();
	},
	updateFold: function(value) {
		this.values.fold = 0.001 * value;
		document.getElementById("ShaperFoldValue").innerHTML = Math.floor(100 * this.values.fold) + "." + (Math.floor(1000 * this.values.fold) % 10);
		this.redraw();
	},
	updateAsym: function(value) {
		this.values.asym = 0.001 * value;
		document.getElementById("ShaperAsymValue").innerHTML = Math.floor(100 * this.values.asym) + "." + (Math.floor(1000 * this.values.asym) % 10);
		this.redraw();
	},
	wrap: function(value) {
		return(value - Math.round(value));
	},
	generateSine: function(value) {
		phase = 0.5 - Math.abs(2 * this.wrap(value - 0.25));
		p2 = phase * phase;
		par = ((2.26548 * p2 - 5.13274) * p2 + 3.14159) * phase;
		return(par);
	},
	generateShape: function(value) {
		ctrl = value * (this.values.drive_f + 0.18);
		weightedCtrl = ctrl * (1);
		sig = this.generateSine(weightedCtrl);
		if (ctrl > 0.25) {
			val = this.values.fold * (sig - 1) + 1;
		} else if (-0.25 > ctrl) {
			val = this.values.fold * (sig + 1) - 1;
		} else {
			val = sig;
		}
		res = (val * (1 - this.values.asym)) + (2 * (val * val - 0.5) * this.values.asym);
		return(res);
	},
	generateShapeNormed: function(value) {
		ctrl = value * (0.18);
		weightedCtrl = ctrl * (1);
		sig = this.generateSine(weightedCtrl);
		if (ctrl > 0.25) {
			val = this.values.fold * (sig - 1) + 1;
		} else if (-0.25 > ctrl) {
			val = this.values.fold * (sig + 1) - 1;
		} else {
			val = sig;
		}
		res = (val * (1 - this.values.asym)) + (2 * (val * val - 0.5) * this.values.asym);
		return(res);
	},
	redraw: function() {
		this.cnv.width = Math.floor(window.getComputedStyle(this.cnv, null).getPropertyValue("width").replace("px",""));
		this.cnv.height = Math.floor(window.getComputedStyle(this.cnv, null).getPropertyValue("height").replace("px",""));
		
		this.ctx.clearRect(0, 0, this.cnv.width, this.cnv.height);
		
		this.ctx.textAlign = "center";
		this.ctx.textBaseline = "middle";
		this.ctx.font = "bold 12px Verdana";
		
		this.drawCurve();
		this.drawOutput();
		this.drawInput();
		
	},
	
	drawCurve: function() {
		
//		Curve diagram (outline+axes)
		this.ctx.strokeStyle = "rgba(100,100,100,1)";
//		Curve Typo
		this.ctx.fillStyle = "rgba(200,200,200,1)";
		
		x = 0.5;
		y = Math.floor(0.55 * this.cnv.height) + 0.5
		w = Math.floor(0.6 * this.cnv.width);
		h = Math.floor(0.45 * this.cnv.height);
		c = Math.floor(0.5 * w) + x;
		
		this.ctx.strokeRect(x, 0.5, w, h);
		this.ctx.fillText("Curve", x + 0.5 * w, 12);
		
		p = 5;
		w -= 2 * p;
		n = Math.floor(0.5 * h) + 0.5;
		a = 0.5 * n;
		r = 8.25;
		
		this.ctx.beginPath();
		this.ctx.moveTo(p, n);
		this.ctx.lineTo(p + w, n);
		this.ctx.stroke();
		
		this.ctx.beginPath();
		this.ctx.moveTo(c, n - 32);
		this.ctx.lineTo(c, n + 32);
		this.ctx.stroke();

//		Curve Graph		
		this.ctx.strokeStyle = "rgba(210, 200, 165, 1)";
// 		this.ctx.strokeStyle = "rgba(64,128,255,0.75)";
		this.ctx.beginPath();
		this.ctx.moveTo(p,n - (a * this.generateShapeNormed(-1 * r)));
		for(var i = 0; i < w; i++) {
			px = p + i;
			vx = r * ((2 * i / w) - 1);
			py = n - (a * this.generateShapeNormed(vx));
			this.ctx.lineTo(px, py);
		};
		this.ctx.stroke();
		
	},
	drawInput: function() {
		
//		Curve diagram (outline+axes)
		this.ctx.strokeStyle = "rgba(100,100,100,1)";
//		Curve Typo
		this.ctx.fillStyle = "rgba(200,200,200,1)";
		
		x = 0.5;
		y = Math.floor(0.55 * this.cnv.height) + 0.5;
		w = Math.floor(0.6 * this.cnv.width);
		h = Math.floor(0.45 * this.cnv.height);
		c = Math.floor(0.5 * w) + x;
		
		this.ctx.strokeRect(x, y, w, h);
		this.ctx.fillText("Input", x + 0.5 * w, y + 12);
		
		p = 5;
		w -= 2 * p;
		n = Math.floor(y + 0.5 * h) + 0.5;
		
		this.ctx.beginPath();
		this.ctx.moveTo(p, n);
		this.ctx.lineTo(p + w, n);
		this.ctx.stroke();
		
		this.ctx.beginPath();
		this.ctx.moveTo(c, n - 32);
		this.ctx.lineTo(c, n + 32);
		this.ctx.stroke();
		
		corr = 0.34;
		
//		Input Graph		
		this.ctx.strokeStyle = "rgba(210, 200, 165, 1)";
//		this.ctx.strokeStyle = "rgba(64,128,255,0.75)";
		
		this.ctx.rect(x, 0, w + 2 *p, this.cnv.height);
		this.ctx.clip();
		
		this.ctx.beginPath();
		this.ctx.moveTo(c, n -32);
		for(var i = 0; i <= 64; i++) {
			vx = c - (this.values.drive_f + 0.18) * corr * w * this.generateSine((i - 32) / 64);
			this.ctx.lineTo(vx, n - 32 + i);
		};
		this.ctx.stroke();
//		Drive range border		
		this.ctx.strokeStyle = "rgba(95,140,230,1)";
		this.ctx.beginPath();
		this.ctx.moveTo(c - (this.values.drive_f + 0.18) * corr * w, 0);
		this.ctx.lineTo(c - (this.values.drive_f + 0.18) * corr * w, this.cnv.height);
		this.ctx.stroke();
		this.ctx.beginPath();
		this.ctx.moveTo(c + (this.values.drive_f + 0.18) * corr * w, 0);
		this.ctx.lineTo(c + (this.values.drive_f + 0.18) * corr * w, this.cnv.height);
		this.ctx.stroke();
		
	},
	drawOutput: function() {

//		Output diagram (outline+axes)
		this.ctx.strokeStyle = "rgba(100,100,100,1)";
//		Output Typo
		this.ctx.fillStyle = "rgba(200,200,200,1)";
		
		x = Math.floor(0.65 * this.cnv.width) + 0.5;
		y = Math.floor(0.55 * this.cnv.height) + 0.5
		w = Math.floor(0.34 * this.cnv.width);
		h = Math.floor(0.45 * this.cnv.height);
		c = Math.floor(0.5 * w) + x;
		
		this.ctx.strokeRect(x, 0.5, w, h);
		this.ctx.fillText("Output", x + 0.5 * w, 12);
		
		p = 5;
		w -= 2 * p;
		n = Math.floor(0.5 * h) + 0.5;
		
		this.ctx.beginPath();
		this.ctx.moveTo(x + p, n);
		this.ctx.lineTo(x + p + w, n);
		this.ctx.stroke();
		
		this.ctx.beginPath();
		this.ctx.moveTo(c, n - 32);
		this.ctx.lineTo(c, n + 32);
		this.ctx.stroke();

//		Output Graph		
		this.ctx.strokeStyle = "rgba(210, 200, 165, 1)";
//		this.ctx.strokeStyle = "rgba(64,128,255,0.75)";
		this.ctx.beginPath();
		this.ctx.moveTo(x + p,n - (a * this.generateShape(0)));
		for(var i = 0; i < w; i++) {
			px = p + i;
			vx = 0.5 * ((2 * i / w) - 1);
			py = n - (a * this.generateShape(this.generateSine(vx)));
			this.ctx.lineTo(x + px, py);
		};
		this.ctx.stroke();
		
	},
};

window.onload = function() {
	shaper.init();
}