function formatNote(note) {
  var keyLabels = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
  var OctaveCount = 5;
  var KeysPerOctave = 12;
  var KeyCount = OctaveCount * KeysPerOctave;

  note = Math.max(0, Math.min(note, KeyCount));
  var index = note % KeysPerOctave;
  var base = keyLabels[parseInt(index)];
  var octave = Math.floor(note / KeysPerOctave) + 1;

  return base + octave;
}

function formatOnOff(cp) {
  return Number(cp) <= Number(1) ? "Off" : "On";
}

function formatPriority(cp) {
  switch (Number(cp)) {
    case 1:
      return "Lowest";
    case 2:
      return "Latest";
    case 3:
      return "Highest";
  }
}

function formatDimensionRounded(value, unit, withUnit, roundTo) {
  if (value === Number.POSITIVE_INFINITY)
    return "inf";

  var cnt = 0;

  var valueIsNegative = value < 0;
  value = Math.abs(value);

  while (Math.round(roundTo) < 1 && roundTo !== 0) {
    roundTo *= 10;
    value *= 10;
    cnt++;
  }

  roundTo = Math.round(roundTo);

  value /= roundTo;
  value = Math.round(value);

  if (value === -0.0)
    value = 0.0;

  value *= roundTo;
  value *= Math.pow(10, -cnt);

  if (valueIsNegative)
    value = -1.0 * value;

  var num = Number(value);
  var ret = num.toFixed(cnt);

  if (withUnit)
    return ret + " " + unit;
  else
    return ret + "";
}

function formatDimensionDigits(value, unit, withUnit, numDigits) {
  if (value === Number.POSITIVE_INFINITY)
    return "inf";

  var intermediate = Number(value).toPrecision(numDigits);
  var ret = parseFloat(intermediate);

  if (withUnit)
    return ret + " " + unit;
  else
    return ret + "";
}

var stingizerMap = new Map();

function formatDimension(value, stringizer, withUnit) {
  var key = stringizer;
  var cachedFn = stingizerMap.get(key);

  if (cachedFn === undefined) {
    var fnBody = "{ return " + stringizer + "; }";
    var fn = new Function("cpValue", "withUnit", fnBody);
    stingizerMap.set(key, fn);
    cachedFn = fn;
  }

  return cachedFn(value, withUnit);
}
