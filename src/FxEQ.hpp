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

#ifndef FXEQ_HPP_
#define FXEQ_HPP_

#include "Fx.hpp"
#include "BiquadPeakFilter.hpp"

class FxEQ : public Fx
{
public:
	FxEQ () = delete;

	FxEQ (RingBuffer<Stereo>** buffer, float* params, Pad* pads, const double rate) :
		Fx (buffer, params, pads),
		rate (rate),
		gains {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}

	{
		for (int i = 0; i < 6; ++i) filters[i] = BiquadPeakFilter (rate);
		filters[0].set (30.0f, M_SQRT1_2, 0.0f);	// Sub
		filters[1].set (80.0f, 1.0, 0.0f);	// Boom
		filters[2].set (300.0f, M_SQRT1_2, 0.0f);			// Warmth
		filters[3].set (1500.0f, 1.0, 0.0f);	// Clarity
		filters[4].set (4000.0f, 1.0, 0.0f);	// Presence
		filters[5].set (15000.0f, 1.0, 0.0f);	// Air
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		for (int i = 0; i < 6; ++i)
		{
			const double r = bidist (rnd);
			gains[i] = 72.0f * LIMIT (params[SLOTS_OPTPARAMS + 2 * i] + r * params[SLOTS_OPTPARAMS + 2 * i + 1], 0.0f, 1.0f) - 36.0f;
			filters[i].setPeakGain (gains[i]);
		}
	}

	virtual Stereo process (const double position, const double size) override
	{
		Stereo s = (**buffer).front();
		for (int i = 0; i < 6; ++i) s = filters[i].process (s);
		return s;
	}

protected:
	double rate;
	float gains[6];
	BiquadPeakFilter filters[6];
};

#endif /* FXEQ_HPP_ */
