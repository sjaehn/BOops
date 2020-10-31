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

#ifndef FXBITCRUSH_HPP_
#define FXBITCRUSH_HPP_

#include "Fx.hpp"

#define FX_BITCRUSH_LIMIT 0
#define FX_BITCRUSH_LIMITRAND 1
#define FX_BITCRUSH_BIT 2
#define FX_BITCRUSH_BITRAND 3

class FxBitcrush : public Fx
{
public:
	FxBitcrush () : FxBitcrush (nullptr, nullptr, nullptr) {}

	FxBitcrush (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		limit (1.0), bit (16.0), f (65536) {}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		limit = (params ? LIMIT (0.01 + 1.99 * params[SLOTS_OPTPARAMS + FX_BITCRUSH_LIMIT] + 1.99 * r1 * params[SLOTS_OPTPARAMS + FX_BITCRUSH_LIMITRAND], 0.01, 2.0) : 1.0);
		const double r2 = bidist (rnd);
		bit = (params ? LIMIT (1.0 + 32.0 * (params[SLOTS_OPTPARAMS + FX_BITCRUSH_BIT] + r2 * params[SLOTS_OPTPARAMS + FX_BITCRUSH_BITRAND]), 1.0, 32.0) : 16.0);
		f = pow (2, bit - 1);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const float l1 = LIMIT (s0.left + limit, 0, 2.0 * limit) / (2.0 * limit);
		const float l2 = round (l1 * f);
		const float l3 = (l2 - 0.5 * f) * 2.0 * limit / f;
		const float r1 = LIMIT (s0.right + limit, 0, 2.0 * limit) / (2.0 * limit);
		const float r2 = round (r1 * f);
		const float r3 = (r2 - 0.5 * f) * 2.0 * limit / f;

		return mix (s0, Stereo (l3, r3), position, size, mixf);
	}

protected:
	float limit;
	int bit;
	float f;
};

#endif /* FXBITCRUSH_HPP_ */
