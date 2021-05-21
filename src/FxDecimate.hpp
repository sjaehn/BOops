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

#ifndef FXDECIMATE_HPP_
#define FXDECIMATE_HPP_

#include "Fx.hpp"
#include <array>

#define FX_DECIMATE_DECIMATE 0
#define FX_DECIMATE_DECIMATERAND 1

class FxDecimate : public Fx
{
public:
	FxDecimate () = delete;

	FxDecimate (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		decimate (0.0f), stack {0.0, 0.0}, live {0.0, 0.0}, count (0)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r = bidist (rnd);
		decimate = LIMIT (0.01 + 0.99 * (params[SLOTS_OPTPARAMS + FX_DECIMATE_DECIMATE] + r * params[SLOTS_OPTPARAMS + FX_DECIMATE_DECIMATERAND]), 0.01, 1.0);
		stack = Stereo {0.0, 0.0};
		live = Stereo {0.0, 0.0};
		count = 0;
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		if (count + 1.0 >= 1.0 / decimate)
		{
			double c0 = 1.0 / decimate - count;
			stack += s0 * c0;
			live = stack * decimate;
			count = 1.0 - c0;
			stack = s0 * count;
		}

		else
		{
			++count;
			stack += s0;
		}

		return mix (s0, live, position, size, mixf);
	}

protected:
	float decimate;
	Stereo stack;
	Stereo live;
	double count;
};

#endif /* FXDECIMATE_HPP_ */
