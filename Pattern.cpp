/*
 * Pattern.cpp
 *
 *  Created on: May 28, 2018
 *      Author: sebsouza
 */

#include "Pattern.h"

Pattern::Pattern(unsigned char *pTrack0, unsigned char *pTrack1,
		unsigned char *pTrack2, unsigned char *pTrack3,
		unsigned char *pVelocity, unsigned char length) {
	this->pTrack0 = pTrack0;
	this->pTrack1 = pTrack1;
	this->pTrack2 = pTrack2;
	this->pTrack3 = pTrack3;
	this->pVelocity = pVelocity;
	this->length = length;

}

unsigned char Pattern::getLength() const {
	return length;
}

unsigned char Pattern::getTrigger(unsigned char track, unsigned char step) {
	unsigned char trigger = 0;
	if (step < length) {
		switch (track) {
		case 0:
			if (pTrack0 != 0)
				trigger = pTrack0[step];
			break;
		case 1:
			if (pTrack1 != 0)
				trigger = pTrack1[step];
			break;
		case 2:
			if (pTrack2 != 0)
				trigger = pTrack2[step];
			break;
		case 3:
			if (pTrack3 != 0)
				trigger = pTrack3[step];
			break;
		}
	}
	return trigger;
}
void Pattern::toggleTrigger(unsigned char track, unsigned char step) {
	if (step < length) {
		switch (track) {
		case 0:
			if (pTrack0[step] == 0)
				pTrack0[step] = pVelocity[track];
			else
				pTrack0[step] = 0;
			break;
		case 1:
			if (pTrack1[step] == 0)
				pTrack1[step] = pVelocity[track];
			else
				pTrack1[step] = 0;
			break;
		case 2:
			if (pTrack2[step] == 0)
				pTrack2[step] = pVelocity[track];
			else
				pTrack2[step] = 0;
			break;
		case 3:
			if (pTrack3[step] == 0)
				pTrack3[step] = pVelocity[track];
			else
				pTrack3[step] = 0;
			break;
		}
	}
}
