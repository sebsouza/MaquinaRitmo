/*
 * Sequencer.cpp
 *
 *  Created on: May 13, 2018
 *      Author: sebsouza
 */

#include "Sequencer.h"
#include "Pattern.h"

/*
 * Sequencer es responsable de tocar cada paso
 */

ISR(TIMER1_COMPA_vect) {
	Sequencer::OCR1A_ISR();
}

void Sequencer::setup() {
	// Set up Timer 1 output compare interrupt A
	// Timer 1 is basically used as a scheduler that triggers at a regular interval for use to update the synth outputs.
	TCCR1B = 0x02;          // set the timer prescaler to 8 = 16/8 = 2MHz
	SET(TIMSK1, OCIE1A);   // Enable output compare match interrupt on OCR1A
//	sei();

	// Set up Timer 0 for Audio Output
	TCCR0A = 0B10110011;
	TCCR0B = 0x01;          // Set to clock frequency, no prescaler
	OCR0B = OCR0A = 127;
	SET(DDRD, PIN_AUDIO_OUT);

	//TLC Init
	TLC5940_Init();
//	// Default all channels to off
	TLC5940_SetAllGS(0);
//	// Enable Global Interrupts
	sei();

	//ShiftIn 165
	pinMode(SerialInputPin165, INPUT); //dataPin
	pinMode(LoadPin165, OUTPUT); //latchPin
	pinMode(Clock165, OUTPUT); //clockPin
	Serial.begin(9600);

//SPI.begin();

	//Sequencer Init
	setBpm(INITIAL_BPM);
	setCurrentTrack(0);
	setSeqState(1);
	resetTrackSolo();
//	setTrackSolo(0); // prueba

}

void Sequencer::setBpm(unsigned char bpm) {
	seqCounterStart = (UPDATE_RATE / ((float) bpm / 60.0));
}

bool Sequencer::stepComplete() {

	bool oldStepComplete = fStepComplete;
	if (oldStepComplete) {
		fStepComplete = false;
	}
	return oldStepComplete;
}

void Sequencer::OCR1A_ISR() {

	OCR1A += (TIMER1_FREQUENCY / UPDATE_RATE); // set timer to come back for the next update in 125us time

	/*
	 * Synth Amplitude Envelope
	 */
	bool updateEnvelope = false;
	if (amplitudeEnvelopeDivider == 0) {
		amplitudeEnvelopeDivider = AMPLITUDE_ENVELOPE_DIVIDER;
		updateEnvelope = true;
	} else {
		amplitudeEnvelopeDivider--;
	}

	/*
	 * Synth Pitch Envelope
	 */
	bool applyPitchEnvelope = (pitchEnvelopeDivider == 0);
	if (applyPitchEnvelope == 0) {
		pitchEnvelopeDivider = PITCH_ENVELOPE_DIVIDER;
	} else {
		pitchEnvelopeDivider--;
	}

	/*
	 * Audio Output
	 */

	if (seqState) {
		OCR0B = 127;
		for (int track = 0; track < NUM_TRACKS; ++track) {
			if ((1 << track) & trackSoloMask)
				OCR0B += (voice[track].getSample(updateEnvelope,
						applyPitchEnvelope));
		}
	}

	/*
	 * Step Sequencer Timer
	 */
	if (seqCounter == 0) {
		seqCounter = seqCounterStart;
		fStepComplete = true;
	} else if (seqCounter == (seqCounterStart / 4)) {
		fStepComplete = true;
	} else if (seqCounter == (seqCounterStart / 3)) {
		fStepComplete = true;
	} else if (seqCounter == (seqCounterStart / 2)) {
		fStepComplete = true;
	} else if (seqCounter == 2 * (seqCounterStart / 3)) {
		fStepComplete = true;
	} else if (seqCounter == 3 * (seqCounterStart / 4)) {
		fStepComplete = true;
	}
	seqCounter--;
}

void Sequencer::updateVisualiser(Pattern& currentPattern) {
	// for each channel we have an 8-bit power level - its the amplitude
	// lets divide by 4 to give 0-15
	// lets decrement each pass, if high byte set, set top led, keep the bottom led on as long as the value is > 1
//	static unsigned char refreshDivider = REFRESH_DIVIDER;
//	static unsigned char channelPower;

//	if (refreshDivider == 0) {
////		if (!(currentPattern.getTrigger(currentTrack, currentStep) == 0)) {
////			channelPower = (voice[currentTrack].getAmplitude() >> 4);
//			channelPower = 0x1000;
////		}
//	}

	TLC5940_SetAllGS(0);
	for (int i = 0; i < currentPattern.getLength(); ++i) {
		if (seqState) {
			if (i == currentStep) {
				if (currentPattern.getTrigger(currentTrack, i))
					TLC5940_SetGS(i, 0);
				else
					TLC5940_SetGS(i, 1024);
			} else if (currentPattern.getTrigger(currentTrack, i))
				TLC5940_SetGS(i, 4095);
		} else if (currentPattern.getTrigger(currentTrack, i))
			TLC5940_SetGS(i, 4095);
	}

	TLC5940_SetGSUpdateFlag();

//	if (channelPower > 0)
//		channelPower--;
//
//	refreshDivider--;
}

void Sequencer::setCurrentTrack(unsigned char track) {

	currentTrack = track;
}

void Sequencer::setTrackSolo(unsigned char track) {

	trackSoloMask = (1 << track);
}

void Sequencer::resetTrackSolo() {

	trackSoloMask = 0xFFFF;
}

void Sequencer::toggleState() {
	seqState = !seqState;
}

void Sequencer::setSeqState(unsigned char state) {
	if (state == 0)
		currentStep = 0;
	seqState = state;
}

void Sequencer::scanButtons(Pattern& currentPattern) {
	cli();
	digitalWrite(Clock165, HIGH);
//	asm("nop\n nop\n");
	digitalWrite(LoadPin165, LOW);
//	asm("nop\n nop\n");
	digitalWrite(LoadPin165, HIGH);
//	asm("nop\n nop\n");
	unsigned int seqButtonScan = shiftIn(SerialInputPin165, Clock165, MSBFIRST); /*TODO:Reemplazar por FastShiftIn*/
	sei();

//	Serial.println(seqButtonScan, BIN);

	for (int i = 0; i < 8/*VER*/; ++i) {
		if (((seqButtonScan >> i) & (~lastSeqButtonScan >> i)) & 1) {
			currentPattern.toggleTrigger(currentTrack, i);
//			Serial.println(currentPattern.getTrigger(currentTrack,i));
		}
	}

	lastSeqButtonScan = seqButtonScan;

}

void Sequencer::loop(Pattern& currentPattern) {
	if (!gsUpdateFlag)
	{
		scanButtons(currentPattern);
		updateVisualiser(currentPattern);

	}

	if (seqState) {
		if (stepComplete()) {
//			setBpm(INITIAL_BPM/*TODO analog read*/);
			for (unsigned char i = 0; i < NUM_TRACKS; i++) {
				unsigned char note = currentPattern.getTrigger(i, currentStep);
				if (note) {
					voice[i].trigger();
				}
			}
			currentStep++;
			if (currentStep == currentPattern.getLength()) {
				currentStep = 0;
				/*TODO current page ++ */
			}
		}
	}

}

static unsigned char Sequencer::currentStep = 0;
static unsigned char Sequencer::currentTrack = 0;
static unsigned char Sequencer::seqState = 0;
static unsigned int Sequencer::lastSeqButtonScan = 0xFFFF;
static volatile unsigned char Sequencer::trackSoloMask = 0;
static volatile unsigned int Sequencer::seqCounterStart = 0;
static volatile unsigned int Sequencer::seqCounter = 0;
static volatile bool Sequencer::fStepComplete = false;
static volatile unsigned char Sequencer::amplitudeEnvelopeDivider =
AMPLITUDE_ENVELOPE_DIVIDER;
static volatile unsigned int Sequencer::pitchEnvelopeDivider =
PITCH_ENVELOPE_DIVIDER;

Sequencer::Voice Sequencer::voice[NUM_TRACKS];

Sequencer::Voice::Voice() {
	envelopeTableStart = 0;
	envelopePhaseAccumulator = 0x8000;
	envelopePhaseIncrement = 10;
	amplitude = 255;
	envelopePitchModulation = 0;
	waveTableStart = 0;
	wavePhaseAccumulator = 0;
	wavePhaseIncrement = 1000;
	pitch = 500;

}

void Sequencer::Voice::setup(unsigned int waveform, float pitch,
		unsigned int envelope, float length, unsigned int mod) {
	// do the maths before we turn off interrupts
	unsigned int tempEnvelopePhaseIncrement = (1.0 / length)
			/ (SAMPLE_RATE / (32767.5 * 10.0)); //[s];
	pitch = pitch / (SAMPLE_RATE / TIMER1_MAX); //[Hz] // based for pitch adjustment - transpose ?

	// turn off interrupts and copy the calculated values into the voice
	unsigned char sreg = SREG;
	cli();

	waveTableStart = waveform; //[address in program memory]
	envelopeTableStart = envelope; //[address in program memory]
	envelopePhaseIncrement = tempEnvelopePhaseIncrement;
	this->pitch = pitch;
	envelopePitchModulation = mod; //0-1023 512=no mod
	SREG = sreg;
//	sei();
}

//void Sequencer::Voice::setWaveform(unsigned int waveData) {
//}
//
//void Sequencer::Voice::setEnvelope(unsigned int envelopeData) {
//}

void Sequencer::Voice::trigger() {
	envelopePhaseAccumulator = 0;

}

signed char Sequencer::Voice::getSample(bool updateEnvelope,
bool applyEnvelopePitchModulation) {
	// calculate the amplitude based on the position within the enveloped
	if (updateEnvelope) {
		if (!(envelopePhaseAccumulator & 0x8000)) {
			// amplitude = envelope position determined by adding envelope increment to envelope accumulator
			amplitude = pgm_read_byte(
					envelopeTableStart + ((envelopePhaseAccumulator +=
							envelopePhaseIncrement) >> 7));

			if (envelopePhaseAccumulator & 0x8000) {
				amplitude = 0;
			}
		} else {
			amplitude = 0;
		}
	}

	if (applyEnvelopePitchModulation) {
		// this works
		wavePhaseIncrement = pitch
				+ (pitch * (envelopePhaseAccumulator / (32767.5 * 128.0))
						* ((int) envelopePitchModulation - 512));
		// this probably doesn't, as and when we understand the objective of m_nEnvelopePitchModulation we will rework for integer maths.
		// m_unWavePhaseIncrement=m_unPitch+(m_unPitch*(m_unEnvelopePhaseAccumulator/(32767*128))*((int)m_nEnvelopePitchModulation-512));
	}

	wavePhaseAccumulator += wavePhaseIncrement;

	// if the amplitude is 0, the sample will always be 0 so there is no need to waste time calculating it.
	if (amplitude == 0) {
		return 0;
	}

	// read a byte representing the current point in the waveform from program memory, multiply it by the current amplitude
	// to mix the waveform with the envelope - its so simple, but this is what makes the rich range of sound from a wavetable synth possible

	return (((signed char) pgm_read_byte(
			waveTableStart + ((wavePhaseAccumulator) >> 8)) * amplitude) >> 8);
}

unsigned char Sequencer::Voice::getAmplitude() {
	return amplitude;
}

