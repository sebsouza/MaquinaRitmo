/*
 * Pattern.h
 *
 *  Created on: May 18, 2018
 *      Author: sebsouza
 */

#ifndef PATTERN_H_
#define PATTERN_H_





class Pattern {
	public:
		Pattern();
		Pattern(unsigned char *, unsigned char *, unsigned char *,
				unsigned char *, unsigned char);
		unsigned char getLength() const;
		unsigned char getTrigger(unsigned char, unsigned char);
	protected:
		unsigned char *pTrack0;
		unsigned char *pTrack1;
		unsigned char *pTrack2;
		unsigned char *pTrack3;
		unsigned char length;
	};

#endif /* PATTERN_H_ */
