/* B.Noname01
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

#ifndef FXNOISE_HPP_
#define FXNOISE_HPP_

#include "Fx.hpp"

#ifndef DB2CO
#define DB2CO(x) pow (10, 0.05 * (x))
#endif

#define FX_NOISE_AMP 0
#define FX_NOISE_AMPRAND 1

class FxNoise : public Fx
{
public:
	FxNoise () : FxNoise (nullptr, nullptr, nullptr) {}

	FxNoise (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		amp (0.0f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r = bidist (rnd);
		const float db = -90.0 + 102.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_NOISE_AMP] + r * params[SLOTS_OPTPARAMS + FX_NOISE_AMPRAND], 0.0, 1.0) :0.5);
		amp = DB2CO (db);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		Stereo s1 = Stereo {unidist (rnd) * amp, unidist (rnd) * amp};
		return mix (s0, s1, position, size, mixf);
	}

protected:
	float amp;

};

#endif /* FXNOISE_HPP_ */
