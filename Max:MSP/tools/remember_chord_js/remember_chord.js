post("Iʻm stoked about this");

inlets = 4;
outlets = 2;

chordOver = false;

var chord = [];
var lastChord = [];
var velocity;

// case switching
var caseNumber = 0;
var index = 0;
var noteRepeats = 4;
var noteRepeatIndex = 0;

function msg_int(a) {
	if (inlet == 0) {
		var note = a;
		if (chordOver == false) {
			post("note: " + note);
			post();
			chord.push(note);
			post("adding to chord" + note);
			post();
			post("current chord: " + chord);
			post();
		}
	}
	if (inlet == 1) {
		// check for repeat 0 velocity
		// first non 0 velocity after the zeros is where to make chordover false
		var newVelocity = a;
		post("new velocity: " + newVelocity);
		post();
		if (newVelocity == velocity && velocity == 0) {
			velocity = newVelocity;
			post("repeat 0 velocity, disregard");
			post();
		} else if (newVelocity != 0 && velocity == 0) {
			velocity = newVelocity;
			post("first non zero velocity");
			post();
			chordOver = false;
			post("chordOver: " + chordOver);
			post();

		} else if (newVelocity == 0 && velocity != 0) {
			velocity = newVelocity;
			chordOver = true;
			post("non-repeat 0 velocity, thus chordOver is true");
			post();
			post("final chord: " + chord);
			post();
			lastChord = chord.slice();
			post("move final chord to lastChord: " + lastChord);
			post();
			chord.length = 0;
			post("emtpy chord: " + chord);
			post();

		}

	}
	if (inlet == 3) {
		a = arguments[0];
		if (a < 0) {a = 0};
		if (a > 4) {a = 4}; // increase this # if increase cases 
		caseNumber = a;
		post("case switch: " + caseNumber);
		post();
	}
}

function bang() {
			//case 5 = polyphony?
			//case 6 = even and odd array items?
	switch (caseNumber) {
		case 0:
			// iterate through all notes in the chord
			if (index < (lastChord.length - 1)) {
				index++;
			} else {
				index = 0;
			}
			outlet(0, lastChord[index]);
			break;
		case 1:
			// case 1 kind of runs slow
			if (index > 0) {
				index = index - 1;
			} else {
				index = lastChord.length;
			}
			outlet(0, lastChord[index]);
			break;
		case 2:
			// repeated notes
			if (noteRepeatIndex < noteRepeats - 1) {
				noteRepeatIndex++
			} else {
				noteRepeatIndex = 0;
					if (index < (lastChord.length - 1)) {
					index++;
				} else {
					index = 0;
				}
			}
			outlet(0, lastChord[index]);
			break;
		case 3:
			// random
			index = Math.floor(Math.random() * lastChord.length);
			outlet(0, lastChord[index]);
			break;
		case 4:
			// random + random octave up or down
			index = Math.floor(Math.random() * lastChord.length);
			var octave = (Math.floor(Math.random() * 3)-1);
			var note = lastChord[index]+(octave*12);
			outlet(0, note);
			break;
		//case 5:
		//	break;
		//case 6:
		//	break;
	}
}