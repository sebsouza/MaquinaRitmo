#include "MaquinaRitmo.h"

Pattern currentPattern(initPattern[0], initPattern[1], initPattern[2], initPattern[3],
		initVelocity, initLenght);

void setup() {


	Sequencer::setup();
	Sequencer::voice[0].setup((unsigned int) SinTable, 60.0,
			(unsigned int) Env1, 1.0, 300);
	Sequencer::voice[1].setup((unsigned int) TriangleTable, 500.0,
			(unsigned int) Env1, 1.0, 512);
	Sequencer::voice[2].setup((unsigned int) RampTable, 300.0,
			(unsigned int) Env2, .5, 1000);
	Sequencer::voice[3].setup((unsigned int) NoiseTable, 1200.0,
			(unsigned int) Env3, .02, 5000);
}

void loop() {
Sequencer::loop(currentPattern);
}
