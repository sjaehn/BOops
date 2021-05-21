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

#ifndef FXAMP_HPP_
#define FXAMP_HPP_

#include "Fx.hpp"

#define FX_AMP_AMP 0
#define FX_AMP_AMPRAND 1

class FxAmp : public Fx
{
public:
	FxAmp () = delete;

	FxAmp (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		amp (0.0f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r = bidist (rnd);
		amp = LIMIT (2.0 * (params[SLOTS_OPTPARAMS + FX_AMP_AMP] + r * params[SLOTS_OPTPARAMS + FX_AMP_AMPRAND]), 0.0, 2.0);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		return mix (s0, s0 * amp, position, size, mixf);
	}

protected:
	float amp;

};

#endif /* FXAMP_HPP_ */
