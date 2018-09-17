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
#include "Tlc5940.h"

//#include "FastShiftIn.h"




#define LoadPin165  6  // - Enable Chip Read / Clock
#define SerialInputPin165  12    // - Read Input Port to SPI Register
#define Clock165 7

#define SET(x,y) (x |=(1<<y))		        					//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       						// |
#define CHK(x,y) (x & (1<<y))           						// |
#define TOG(x,y) (x^=(1<<y))            						//-+

#define TIMER1_FREQUENCY 2000000
#define UPDATE_RATE 8000

#define NUM_TRACKS 4
#define NUM_STEPS 24
#define NUM_PATTERNS 16
#define INITIAL_BPM 120

#define SAMPLE_RATE 16000.0
#define TIMER1_MAX 65535
#define AMPLITUDE_ENVELOPE_DIVIDER 4           // This is similar to a prescaler, we do not update the envelope every cycle we do it ever cycle/ENVELOPE_DIVIDER
#define PITCH_ENVELOPE_DIVIDER 800 // This is the same concept as above, we update the modulation pitch every cycle/MODULATION_PITCH_DIVIDER

#define PIN_AUDIO_OUT 5

// Pins and other definitions for the LED Visualiser
#define REFRESH_DIVIDER 16
#define CHANNEL_MAX 4


class Sequencer {

public:

	static unsigned char currentStep;
	static unsigned char currentTrack;
	static unsigned char seqState;
	static unsigned int lastSeqButtonScan;

	class Voice;
	static Voice voice[NUM_TRACKS];

	Sequencer();
	static void setup();
	static void loop(Pattern&);
	// simple counters that can be used outside Sequencer for sequencing
	// stepComplete will return true if a new bar has been completed
	static void OCR1A_ISR() __attribute__((always_inline));
	static void setBpm(unsigned char bpm);
	static bool stepComplete();
	static void updateVisualiser(Pattern&);
	static void setTrackSolo(unsigned char);
	static void resetTrackSolo();
	static void setCurrentTrack(unsigned char);
	static void toggleState();
	static void setSeqState(unsigned char);
	static void scanButtons(Pattern&);

protected:
	static volatile unsigned int seqCounterStart; // m_unBarCounter counts down from barCounterStart
	static volatile unsigned int seqCounter; //- on zero sets m_sBar to indicate a bar at the required BPM has completed and starts the next count down from barCounterStart
	static volatile bool fStepComplete; //- Flags that a bar is complete, can be ignored or used by user code to trigger a new bar automatically - accessed through barComplete function
	static volatile unsigned char amplitudeEnvelopeDivider; //- We update the envelope every fourth ENVELOPE_DIVIDER, this counts down from ENVELOPE_DIVIDER to 0 and is used to update the envelope at 0 before staring another countdown from ENVELOPE_DIVIDER
	static volatile unsigned int pitchEnvelopeDivider; // We update envelope pitch modulations every MODULATION_PITCH_DIVIDER samples, similar to above.
	static volatile unsigned char trackSoloMask;
};

class Sequencer::Voice {
public:
	Voice();
	void setup(unsigned int waveform, float pitch, unsigned int envelope,
			float length, unsigned int mod);
//	void setWaveform(unsigned int waveData);
//	void setEnvelope(unsigned int envelopeData);
	void trigger();
	signed char getSample(bool, bool) __attribute__((always_inline));
	unsigned char getAmplitude();
protected:
	// Waveform related parameters -
	volatile unsigned int waveTableStart; // To assign a wavetable to a voice, all we do is point the m_unWaveTableStart member of the voice to the address of the wave table array in memory
	volatile unsigned int wavePhaseAccumulator; // The WaveTablePhaseAccumulator sound complicated because it is based on established wave table terminology - in reality is just an index into the array pointed to by m_unWaveTableStart
	volatile unsigned int wavePhaseIncrement; // Again we are following established wave table synth terminology - in simpler terms, this is just added to the wave phase accumulator each cycle
											  // a low value means we step through the wave table slowly producing a low frequency bass sound, a high value means we step through more quickly producing
											  // a high frequency treble sound.
	volatile unsigned int pitch; // This is the original pitch assigned to the sound - the synth includes some capabilities to bend a note away from
								 // its original pitch over the duration of the note - we use this to record the original note pitch
	volatile unsigned int envelopeTableStart; // The start of the array representing the envelope
	volatile unsigned int envelopePhaseAccumulator; // The current position in the envelope - Note - unlike the wave form, we only cycle through the envelope once,
													// it describes the life (volume really) of a note from start to finish.
	volatile unsigned int envelopePhaseIncrement; // This controls how fast we move through the envelope table, high values will be fast giving an abrupt note like a drum or percussion
												  // lower values will give a prolonged note.

	volatile unsigned char amplitude; // This records the most recent value read from the wavetable - its a more efficient than reading and calculating each time.
									  // also we do not calculate the amplitude continually, there is a divider - see m_sEnvelopeDivider.
									  // m_sAmplitude will always have a cache of the most recent value
	volatile int envelopePitchModulation; // The allows a note to increase or decrease in pitch as its played, for instance a bass sound that drops as it decays

};

#endif /* SEQUENCER_H_ */
