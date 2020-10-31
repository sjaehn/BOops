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

#ifndef FXSCRATCH_HPP_
#define FXSCRATCH_HPP_

#include "Fx.hpp"
#include "Shape.hpp"

#define FX_SCRATCH_RANGE 0
#define FX_SCRATCH_RANGERAND 1

class FxScratch : public Fx
{
public:
	FxScratch () : FxScratch (nullptr, nullptr, nullptr, nullptr, nullptr) {}

	FxScratch (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, Shape<SHAPE_MAXNODES>* shape) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		shape (shape),
		range (0.0f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
		const double r = bidist (rnd);
		range = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_SCRATCH_RANGE] + r * params[SLOTS_OPTPARAMS + FX_SCRATCH_RANGERAND], 0.0, 1.0) : 0.5);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const double f = (shape ? shape->getMapValue (fmod (position, 1.0)): 0.0);
		const long frame = framesPerStep * range * (-LIMIT (f, -1.0, 0.0));
		Stereo s1 = (buffer && (*buffer) ? (**buffer)[frame] : Stereo {0, 0});
		return mix (s0, s1, position, size, mixf);
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	Shape<SHAPE_MAXNODES>* shape;
	float range;
};

#endif /* FXSCRATCH_HPP_ */
