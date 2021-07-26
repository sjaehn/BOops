/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

// Adapted from https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

#ifndef BIQUAD_HPP_
#define BIQUAD_HPP_

#include <cmath>
#include "Stereo.hpp"

class Biquad
{
public:
	Biquad () : Biquad (48000) {}
	Biquad (const double rate) :
		rate (rate),
		Fc (0.5f),
		Q (M_SQRT1_2),
		gain (0.0f),
		a0 (1.0f), a1 (0.0f), a2 (0.0f),
		b1 (0.0f), b2 (0.0f),
		z1 (0.0f, 0.0f), z2 (0.0f, 0.0f)
	{}

	virtual ~Biquad () {}

	void set (const float freq, const float Q, const float gain)
	{
		Fc = freq / rate;
		this->Q = Q;
		this->gain = gain;
		build();
	}

	void setFrequency (const float freq) 
	{
		Fc = freq / rate;
		build();
	}

	void setQ (const float Q) 
	{
		this->Q = Q;
		build();
	}

	void setPeakGain (const float gain) 
	{
		this->gain = gain;
		build();
	}

	Stereo process (const Stereo& in) 
	{
		Stereo out = in * a0 + z1;
		z1 = in * a1 + z2 - out * b1;
		z2 = in * a2 - out * b2;
		return out;
	}

	float result (const float freq) 
	{
		const float phi = powf (sinf(/*2.0f * */ M_PI * freq /rate), 2);
		float y = 	logf (powf (a0 + a1 + a2, 2) - 4.0f * (a0 * a1 + 4.0f * a0 * a2 + a1 * a2) * phi + 16.0f * a0 * a2 * phi * phi) - 
					logf (powf (1.0f + b1 + b2, 2) - 4.0f * (b1 + 4.0f * b2 + b1 * b2) * phi + 16.0f * b2 * phi * phi);
		y = y * 10.0f / M_LN10;
		if (y < -100) y = -100;
		return y;
    }


protected:
	double rate; 
	float Fc;
	float Q;
	float gain;
	float a0, a1, a2;
	float b1, b2;
	Stereo z1, z2;

	virtual void build() {}
};

#endif /* BIQUAD_HPP_ */
