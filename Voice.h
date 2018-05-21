/*
 * Voice.h
 *
 *  Created on: May 20, 2018
 *      Author: sebsouza
 */

#ifndef VOICE_H_
#define VOICE_H_

#define AMPLITUDE_ENVELOPE_DIVIDER 4           // This is similar to a prescaler, we do not update the envelope every cycle we do it ever cycle/ENVELOPE_DIVIDER
#define PITCH_ENVELOPE_DIVIDER 800 // This is the same concept as above, we update the modulation pitch every cycle/MODULATION_PITCH_DIVIDER

class Voice {
public:
	Voice();
	void setup(unsigned int waveform, float pitch, unsigned int envelope,
			float length, unsigned int mod);
	void setWaveform(unsigned int waveData);
	void setEnvelope(unsigned int envelopeData);
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

	static volatile unsigned char envelopeDivider; //- We update the envelope every fourth ENVELOPE_DIVIDER, this counts down from ENVELOPE_DIVIDER to 0 and is used to update the envelope at 0 before staring another countdown from ENVELOPE_DIVIDER
	static volatile unsigned int pitchEnvelopeDivider; // We update envelope pitch modulations every MODULATION_PITCH_DIVIDER samples, similar to above.

};

#endif /* VOICE_H_ */
