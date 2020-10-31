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

#ifndef FXTAPESTOP_HPP_
#define FXTAPESTOP_HPP_

#include "Fx.hpp"

#define FX_TAPESTOP_REACH 0
#define FX_TAPESTOP_REACHRAND 1
#define FX_TAPESTOP_ORDER 2
#define FX_TAPESTOP_ORDERRAND 3

class FxTapeStop : public Fx
{
public:
	FxTapeStop () : FxTapeStop (nullptr, nullptr, nullptr, 0) {}

	FxTapeStop (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		reach (1.0), order (1.0), r (0.0) {}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		reach = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_TAPESTOP_REACH] + r1 * params[SLOTS_OPTPARAMS + FX_TAPESTOP_REACHRAND], 0.0, 1.0) : 1.0);
		const int startPos = position;
		r = reach * pads[startPos].size;
		const double r2 = bidist (rnd);
		order = (params ? LIMIT (1.0 + 9.0 * (params[SLOTS_OPTPARAMS + FX_TAPESTOP_ORDER] + r2 * params[SLOTS_OPTPARAMS + FX_TAPESTOP_ORDERRAND]), 1.0, 10.0) : 2.0);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const long frame = (log (exp (order * position) + exp (order * reach) - 1) / order - reach) * framesPerStep;
		Stereo s1 = (buffer && (*buffer) ? (**buffer)[frame] : Stereo {0, 0});
		return mix (s0, s1, position, size, mixf);
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	double reach;
	double order;
	double r;
};

#endif /* FXTAPESTOP_HPP_ */
