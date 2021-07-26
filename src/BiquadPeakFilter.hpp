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

#ifndef BIQUADPEAKFILTER_HPP_
#define BIQUADPEAKFILTER_HPP_

#include "Biquad.hpp"

class BiquadPeakFilter : public Biquad
{
public:
	BiquadPeakFilter() : BiquadPeakFilter (48000) {}
	BiquadPeakFilter (const double rate) : Biquad (rate) {}

protected:
	virtual void build() override 
	{
		float norm;
    	const float V = powf (10.0f, fabsf (gain) / 20.0f);
    	const float K = tanf (M_PI * Fc);

		if (gain >= 0) 
		{ 
			norm = 1.0f / (1.0f + 1.0f / Q * K + K * K);
			a0 = (1.0f + V / Q * K + K * K) * norm;
			a1 = 2.0f * (K * K - 1.0f) * norm;
			a2 = (1.0f - V / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1.0f - 1.0f / Q * K + K * K) * norm;
		}

		else 
		{
			norm = 1.0f / (1.0f + V / Q * K + K * K);
			a0 = (1.0f + 1.0f / Q * K + K * K) * norm;
			a1 = 2.0f * (K * K - 1.0f) * norm;
			a2 = (1.0f - 1.0f / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1.0f - V / Q * K + K * K) * norm;
		}
	}
};

#endif /* BIQUADPEAKFILTER_HPP_ */
