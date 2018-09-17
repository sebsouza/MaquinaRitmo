/*
 * MaquinaRitmo.h
 *
 *  Created on: May 21, 2018
 *      Author: sebsouza
 */

#ifndef MAQUINARITMO_H_
#define MAQUINARITMO_H_

#include <Arduino.h>
#include "Sequencer.h"
//#include "Tlc5940.h"

// Include the wavetables, you could add your own as well
#include "sin256.h"
#include "ramp256.h"
#include "saw256.h"
#include "square256.h"
#include "noise256.h"
#include "tria256.h"
// include the envelopes, again you could add more
#include "env0.h"
#include "env1.h"
#include "env2.h"
#include "env3.h"

unsigned char initPattern[4][24] = {
/*Bass*/
{ 100, 0, 000, 000, 0, 000, /*|*/
000, 000, 000, 000, 000, 000, /*|*/
100, 0, 000, 000, 0, 000, /*|*/
000, 000, 000, 000, 000, 000 },

/*Snare*/
{ 000, 000, 000, 000, 000, 000, /*|*/
100, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 000, 000, 000 },

/*HH O*/
{ 000, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 000, 000, 000, /*|*/
000, 000, 000, 000, 000, 000 },

/*HH C*/
{ 0, 0, 0, 1, 0, 0, /*|*/
0, 0, 0, 1, 0, 0, /*|*/
0, 0, 0, 1, 0, 0, /*|*/
0, 0, 0, 1, 0, 0 },

};

unsigned char initVelocity[4] = { 100, 100, 10, 1 };

unsigned char initLenght= 12;

#endif /* MAQUINARITMO_H_ */
