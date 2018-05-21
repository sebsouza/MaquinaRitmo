/*
 * Sequencer.cpp
 *
 *  Created on: May 13, 2018
 *      Author: sebsouza
 */

#include "Sequencer.h"

unsigned char amenBreak[NUM_TRACKS][NUM_STEPS] = {
/*Bass*/
{ 100, 000, 000, 100, 000, 000, /*|*/
000, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 100, 000, 100, /*|*/
000, 000, 000, 000, 000, 000 },
/*Snare*/
{ 000, 000, 000, 000, 000, 000, /*|*/
200, 000, 000, 000, 000, 100, /*|*/
000, 100, 000, 000, 000, 000, /*|*/
500, 000, 000, 000, 000, 100 },
/*HH*/
{ 001, 000, 000, 001, 000, 000, /*|*/
001, 000, 000, 001, 000, 000, /*|*/
001, 000, 000, 001, 000, 000, /*|*/
001, 000, 000, 001, 000, 000 },
/* empty channel - add you own melody or add 001 to play a user selected pitch on channel 0 - upto you. */
{ 0, 0, 0, 0, 0, 0, /*|*/
0, 0, 0, 0, 0, 0, /*|*/
0, 0, 0, 0, 0, 0, /*|*/
0, 0, 0, 0, 0, 0 },

};

ISR(TIMER1_COMPA_vect) {
	Sequencer::OCR1A_ISR();
}

Sequencer::Sequencer() :
		currentStep(0), currentPattern(amenBreak[0], amenBreak[1], amenBreak[2],
				amenBreak[3], 24) {
}

void Sequencer::setup() {
	// Set up Timer 1 output compare interrupt A
	// Timer 1 is basically used as a scheduler that triggers at a regular interval for use to update the synth outputs.
	cli();
	TCCR1B = (1 << CS11) || (1 << CS10); // set the timer prescaler to 8 => 16MHz/8 = 2MHz
	TIMSK1 = 0 || (1 << OCIE1A); // Enable output compare match interrupt on OCR1A
	sei();

	// Set up Timer 0 the output timer
	cli();
	TCCR0A = 0 || (1 << COM0B1) || (1 << WGM01) || (1 << WGM00); // Fast PWM, clear OC0B on Compare Match, set OC0B at Bottom. OC0A Disconnected.
	TCCR0B = 0 || (1 << CS00);   // Set to clock frequency, no prescaler
	OCR0B = 127;  // set in the middle - do we need this ? probably not.
	DDRD |= (1 << PIN_AUDIO_OUT); // Set digital pin 5 to output
	sei();

	setBpm(120);

	/*
	 * TODO CARGAR ARRAY presetPattern[] CON LA EEPROM
	 */

}

void Sequencer::setBpm(uint8_t bpm) {
	seqCounterStart = (SAMPLE_RATE / ((float) bpm / 60.0));
//	seqCounter = seqCounterStart;
//	fStepComplete = false;
}

bool Sequencer::stepComplete() {

	bool oldStepComplete = fStepComplete;
	if (oldStepComplete) {
		fStepComplete = false;
	}
	return oldStepComplete;
}

void Sequencer::OCR1A_ISR() {

	OCR1A += (TIMER1_FREQUENCY / SAMPLE_RATE); // set timer to come back for the next update in 125us time

	/*
	 * Synth Amplitude Envelope
	 */
	bool updateEnvelope = false;
	if (Voice::envelopeDivider == 0) {
		Voice::envelopeDivider = AMPLITUDE_ENVELOPE_DIVIDER;
		updateEnvelope = true;
	} else {
		Voice::envelopeDivider--;
	}

	/*
	 * Synth Pitch Envelope
	 */
	bool applyPitchEnvelope = (Voice::pitchEnvelopeDivider == 0);
	if (applyPitchEnvelope == 0) {
		Voice::pitchEnvelopeDivider = PITCH_ENVELOPE_DIVIDER;
	} else {
		Voice::pitchEnvelopeDivider--;
	}

	/*
	 * Audio Output
	 */
	OCR0B = 127;
	for (int track = 0; track < NUM_TRACKS; ++track) {

		OCR0B += voice[track].getSample(updateEnvelope,
				applyPitchEnvelope) >> 1;
	}

	/*
	 * Step Sequencer Timer
	 */
	if (seqCounter == 0) {
		seqCounter = seqCounterStart;
		fStepComplete = true;
	} else if (seqCounter % (seqCounterStart / 4) == 0) {
		fStepComplete = true;
	} else if (seqCounter == (seqCounterStart / 3)) {
		fStepComplete = true;
	} else if (seqCounter == 2 * (seqCounterStart / 3)) {
		fStepComplete = true;
	}

	seqCounter--;

}

void Sequencer::loop() {
	if (stepComplete()) {

		Serial.print(seqCounter);
		Serial.print("\t");
		Serial.println(currentStep);

		setBpm(120/*TODO analog read*/);
		for (unsigned char i = 0; i < NUM_TRACKS; i++) {
			unsigned char note = currentPattern.getTrigger(i, currentStep);
			if (note) {
				/* TODO play voice track i-th at currentStep */
			}
		}

		currentStep++;
		if (currentStep == currentPattern.getLength()) {
			currentStep = 0;
			/*TODO current page ++ */

		}

	}
}

static volatile unsigned int Sequencer::seqCounterStart = 0;
static volatile unsigned int Sequencer::seqCounter = 0;
static volatile bool Sequencer::fStepComplete = false;

