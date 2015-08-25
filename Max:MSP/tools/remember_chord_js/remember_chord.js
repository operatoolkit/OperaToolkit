post("Iʻm stoked about this");

inlets = 3;
outlets = 2;

chordOver = false;

var chord = [];
var lastChord = [];
var velocity;
var index = 0;

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
}

function bang() {
	// in one variation, index++ and pull note frmo array
	// in another randomly pull note from array
	// go thru even, then odd items in array
	// repeat array notes a specified number of times
	// play two notes at a time
if (index < (lastChord.length-1)) {
	index++;
} else {
	index = 0;
}
outlet(0, lastChord[index]);
}