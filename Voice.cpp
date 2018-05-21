/*
 * Voice.cpp
 *
 *  Created on: May 20, 2018
 *      Author: sebsouza
 */

#include "Voice.h"
#include <avr/pgmspace.h>

Voice::Voice() {
	 envelopeTableStart=0;
	 envelopePhaseAccumulator=0x8000;
	 envelopePhaseIncrement=10;
	 amplitude=255;
	 envelopePitchModulation=0;
	 waveTableStart=0;
	 wavePhaseAccumulator=0;
	 wavePhaseIncrement=1000;
	 pitch=500;

}

void Voice::setup(unsigned int waveform, float pitch, unsigned int envelope,
		float length, unsigned int mod) {
}

void Voice::setWaveform(unsigned int waveData) {
}

void Voice::setEnvelope(unsigned int envelopeData) {
}

void Voice::trigger() {
}

signed char Voice::getSample(bool updateEnvelope,
		bool applyEnvelopePitchModulation) {
	 // calculate the amplitude based on the position within the enveloped
	  if(updateEnvelope)
	  {
	    if(!(envelopePhaseAccumulator&0x8000))
	    {
	      // amplitude = envelope position determined by adding envelope increment to envelope accumulator
	      amplitude=pgm_read_byte(envelopeTableStart + ((envelopePhaseAccumulator+=envelopePhaseIncrement)>>7) );

	      if(envelopePhaseAccumulator&0x8000)
	      {
	        amplitude=0;
	      }
	    }
	    else
	    {
	      amplitude=0;
	    }
	  }

	  if(applyEnvelopePitchModulation)
	  {
	    // this works
	    // m_unWavePhaseIncrement=m_unPitch+(m_unPitch*(m_unEnvelopePhaseAccumulator/(32767.5*128.0  ))*((int)m_nEnvelopePitchModulation-512));
	    // this probably doesn't, as and when we understand the objective of m_nEnvelopePitchModulation we will rework for integer maths.
	    // m_unWavePhaseIncrement=m_unPitch+(m_unPitch*(m_unEnvelopePhaseAccumulator/(32767*128))*((int)m_nEnvelopePitchModulation-512));
	  }

	  wavePhaseAccumulator+=wavePhaseIncrement;

	  // if the amplitude is 0, the sample will always be 0 so there is no need to waste time calculating it.
	  if(amplitude == 0)
	  {
	    return 0;
	  }

	  // read a byte representing the current point in the waveform from program memory, multiply it by the current amplitude
	  // to mix the waveform with the envelope - its so simple, but this is what makes the rich range of sound from a wavetable synth possible

	  return (((signed char)pgm_read_byte(waveTableStart+((wavePhaseAccumulator)>>8))*amplitude)>>8);
}

unsigned char Voice::getAmplitude() {
}

static volatile unsigned char Voice::envelopeDivider = AMPLITUDE_ENVELOPE_DIVIDER;
static volatile unsigned int Voice::pitchEnvelopeDivider = PITCH_ENVELOPE_DIVIDER;
