/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2003-2004 Fredrik Kilander <fk@dsv.su.se>
 * Copyright (C) 2008-2016 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2012 Will Panther <pantherb@setbfree.org>
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2020 - 2021 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef ACEREVERB_HPP_
#define ACEREVERB_HPP_

#include <cmath>
#include <cstring>
#include <new>
//#include <array>

#define RV_NZ 7
#define DENORMAL_PROTECT (1e-14)

class AceReverb
{
public:
	AceReverb ();
	AceReverb (const double rate, const float roomSize, const float inputGain, const float feedback, const float mix);
	AceReverb (const AceReverb& that) = delete;
	~AceReverb ();

	AceReverb& operator= (const AceReverb& that) = delete;

	void clear ();
	void setRoomSize (const float rs);
	void reverb (const float* inbuf0, const float* inbuf1, float* outbuf0, float* outbuf1, size_t n_samples);

protected:
	float* delays[2][RV_NZ]; /**< delay line buffer */
	size_t size[2][RV_NZ];

	float* idx0[2][RV_NZ];	/**< Reset pointer ref delays[]*/
	float* idxp[2][RV_NZ];	/**< Index pointer ref delays[]*/
	float* endp[2][RV_NZ];	/**< End pointer   ref delays[]*/

	float gain[RV_NZ]; /**< feedback gains */
	float yy1_0; /**< Previous output sample */
	float y_1_0; /**< Feedback sample */
	float yy1_1; /**< Previous output sample */
	float y_1_1; /**< Feedback sample */

	int end[2][RV_NZ];

	double rate;		/**< Rate in fps */
	float roomsz;		/**< Room size */
	float inputGain;	/**< Input gain value */
	float fbk;	/**< Feedback gain */
	float wet;	/**< Output dry gain */
	float dry;	/**< Output wet gain */

	int setReverbPointers (int i, int c, const double rate);
};

AceReverb::AceReverb () : AceReverb
(
	48000.0,
	0.75f,
	powf (10.0f, .05f * -20.0f) /* - 20 db */,
	-0.015f,
	0.5f
)
{}

AceReverb::AceReverb (const double rate, const float roomSize, const float inputGain, const float feedback, const float mix) :
	rate (rate),
	roomsz (roomSize),
	inputGain (inputGain),
	fbk (feedback),
	wet (mix),
	dry (1.0f - mix)
{
	int stereowidth = 7;

	/* feedback combfilter */
	setRoomSize (roomSize);

	/* all-pass filter */
	gain[4] = sqrtf (0.5);
	gain[5] = sqrtf (0.5);
	gain[6] = sqrtf (0.5);

	/* delay lines left */
	end[0][0] = 1687;
	end[0][1] = 1601;
	end[0][2] = 2053;
	end[0][3] = 2251;

	/* all pass filters left */
	end[0][4] = 347;
	end[0][5] = 113;
	end[0][6] = 37;

	/* right */
	for (int i = 0; i < RV_NZ; ++i)
	{
		end[1][i] = end[0][i] + stereowidth;
	}

	for (int i = 0; i < RV_NZ; ++i) {
		delays[0][i] = NULL;
		delays[1][i] = NULL;
	}

	yy1_0 = 0.0;
	y_1_0 = 0.0;
	yy1_1 = 0.0;
	y_1_1 = 0.0;

	for (int i = 0; i < RV_NZ; ++i) {
		if (setReverbPointers (i, 0, rate)) throw std::bad_alloc();
		if (setReverbPointers (i, 1, rate)) throw std::bad_alloc();
	}
}

AceReverb::~AceReverb ()
{
	for (int i = 0; i < RV_NZ; ++i) {
		free (delays[0][i]);
		free (delays[1][i]);
	}
}

int AceReverb::setReverbPointers (int i, int c, const double rate)
{
	int e = (end[c][i] * rate / 25000.0);
	e = e | 1;
	size[c][i] = e + 2;
	delays[c][i] = (float*)realloc ((void*)delays[c][i], size[c][i] * sizeof (float));
	if (!delays[c][i]) {
		return -1;
	} else {
		memset (delays[c][i], 0 , size[c][i] * sizeof (float));
	}
	endp[c][i] = delays[c][i] + e + 1;
	idx0[c][i] = idxp[c][i] = &(delays[c][i][0]);

	return 0;
}

void AceReverb::clear ()
{
	y_1_0 = 0;
	yy1_0 = 0;
	y_1_1 = 0;
	yy1_1 = 0;
	for (int i = 0; i < RV_NZ; ++i) {
		for (int c = 0; c < 2; ++c) {
			memset (delays[c][i], 0, size[c][i] * sizeof (float));
		}
	}
}

void AceReverb::setRoomSize (const float rs)
{
	roomsz = rs;
	gain[0] = 0.773 * roomsz;
	gain[1] = 0.802 * roomsz;
	gain[2] = 0.753 * roomsz;
	gain[3] = 0.733 * roomsz;
}

void AceReverb::reverb (const float* inbuf0, const float* inbuf1, float* outbuf0, float* outbuf1, size_t n_samples)
{
	float** const idxp0 = this->idxp[0];
	float** const idxp1 = this->idxp[1];
	float* const* const endp0 = this->endp[0];
	float* const* const endp1 = this->endp[1];
	float* const* const idx00 = this->idx0[0];
	float* const* const idx01 = this->idx0[1];
	const float* const gain = this->gain;
	const float inputGain = this->inputGain;
	const float fbk = this->fbk;
	const float wet = this->wet;
	const float dry = this->dry;

	const float* xp0 = inbuf0;
	const float* xp1 = inbuf1;
	float* yp0 = outbuf0;
	float* yp1 = outbuf1;

	float y_1_0 = this->y_1_0;
	float yy1_0 = this->yy1_0;
	float y_1_1 = this->y_1_1;
	float yy1_1 = this->yy1_1;

	for (size_t i = 0; i < n_samples; ++i) {
		int j;
		float y;
		float xo0 = *xp0++;
		float xo1 = *xp1++;
		if (!std::isfinite(xo0) || fabsf (xo0) > 10.f) { xo0 = 0; }
		if (!std::isfinite(xo1) || fabsf (xo1) > 10.f) { xo1 = 0; }
		xo0 += DENORMAL_PROTECT;
		xo1 += DENORMAL_PROTECT;
		const float x0 = y_1_0 + (inputGain * xo0);
		const float x1 = y_1_1 + (inputGain * xo1);

		float xa = 0.0;
		float xb = 0.0;
		/* First we do four feedback comb filters (ie parallel delay lines,
		 * each with a single tap at the end that feeds back at the start) */

		for (j = 0; j < 4; ++j) {
			y = *idxp0[j];
			*idxp0[j] = x0 + (gain[j] * y);
			if (endp0[j] <= ++(idxp0[j])) {
				idxp0[j] = idx00[j];
			}
			xa += y;
		}
		for (; j < 7; ++j) {
			y = *idxp0[j];
			*idxp0[j] = gain[j] * (xa + y);
			if (endp0[j] <= ++(idxp0[j])) {
				idxp0[j] = idx00[j];
			}
			xa = y - xa;
		}

		y = 0.5f * (xa + yy1_0);
		yy1_0 = y;
		y_1_0 = fbk * xa;

		*yp0++ = ((wet * y) + (dry * xo0));

		for (j = 0; j < 4; ++j) {
			y = *idxp1[j];
			*idxp1[j] = x1 + (gain[j] * y);
			if (endp1[j] <= ++(idxp1[j])) {
				idxp1[j] = idx01[j];
			}
			xb += y;
		}
		for (; j < 7; ++j) {
			y = *idxp1[j];
			*idxp1[j] = gain[j] * (xb + y);
			if (endp1[j] <= ++(idxp1[j])) {
				idxp1[j] = idx01[j];
			}
			xb = y - xb;
		}

		y = 0.5f * (xb + yy1_1);
		yy1_1 = y;
		y_1_1 = fbk * xb;

		*yp1++ = ((wet * y) + (dry * xo1));
	}

	if (!std::isfinite(y_1_0)) { y_1_0 = 0; }
	if (!std::isfinite(yy1_1)) { yy1_0 = 0; }
	if (!std::isfinite(y_1_1)) { y_1_1 = 0; }
	if (!std::isfinite(yy1_1)) { yy1_1 = 0; }

	this->y_1_0 = y_1_0 + DENORMAL_PROTECT;
	this->yy1_0 = yy1_0 + DENORMAL_PROTECT;
	this->y_1_1 = y_1_1 + DENORMAL_PROTECT;
	this->yy1_1 = yy1_1 + DENORMAL_PROTECT;
}

#endif /* ACEREVERB_HPP_ */
