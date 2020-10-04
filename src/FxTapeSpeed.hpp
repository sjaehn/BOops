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

#ifndef FXTAPESPEED_HPP_
#define FXTAPESPEED_HPP_

#include "Fx.hpp"

#define FX_TAPESPEED_SPEED 0
#define FX_TAPESPEED_SPEEDRAND 1

class FxTapeSpeed : public Fx
{
public:
	FxTapeSpeed () : FxTapeSpeed (nullptr, nullptr, nullptr, nullptr) {}

	FxTapeSpeed (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		speed (1.0) {}

	virtual void start (const double position) override
	{
		Fx::start (position);
		const double r = bidist (rnd);
		speed = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_TAPESPEED_SPEED] + r * params[SLOTS_OPTPARAMS + FX_TAPESPEED_SPEEDRAND], 0.0, 1.0) : 1.0);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		const long frame = (1.0 - speed) * framesPerStep * (position - startPos);
		Stereo s1 = (buffer && (*buffer) ? (**buffer)[frame] : Stereo {0, 0});
		s1.mix (s0, 1.0f - pads[startPos].mix);
		return s1.mix (s0, 1.0f - params[SLOTS_MIX] * adsr (position));
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	double speed;
};

#endif /* FXTAPESPEED_HPP_ */
