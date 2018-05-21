#include <Arduino.h>
#include "Sequencer.h"

Sequencer sequencer;

void setup() {

	Serial.begin(9600);//for debugging
	Serial.println("Cargando...");

	sequencer.setup();

}

void loop() {

	sequencer.loop();

}
