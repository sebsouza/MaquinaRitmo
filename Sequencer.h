/*
 * Sequencer.h
 *
 *  Created on: May 13, 2018
 *      Author: sebsouza
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include <Arduino.h>
#include "Pattern.h"
#include "Voice.h"

#define TIMER1_FREQUENCY 2000000
#define SAMPLE_RATE 8000

#define NUM_TRACKS 4
#define NUM_STEPS 24
#define NUM_PATTERNS 16

#define PIN_AUDIO_OUT 5


class Sequencer {

public:
	unsigned char currentStep;
	Pattern currentPattern/*,presetPattern[16]*/;
	Voice voice[NUM_TRACKS];

	Sequencer();
	void setup();
	void loop();
	// simple counters that can be used outside Sequencer for sequencing
	// stepComplete will return true if a new bar has been completed
	static void OCR1A_ISR() __attribute__((always_inline));
	static void setBpm(uint8_t bpm);
	static bool stepComplete();

protected:

	static volatile unsigned int seqCounterStart; // m_unBarCounter counts down from barCounterStart
	static volatile unsigned int seqCounter; //- on zero sets m_sBar to indicate a bar at the required BPM has completed and starts the next count down from barCounterStart
	static volatile bool fStepComplete; //- Flags that a bar is complete, can be ignored or used by user code to trigger a new bar automatically - accessed through barComplete function

};



#endif /* SEQUENCER_H_ */
