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

#ifndef FXDELAY_HPP_
#define FXDELAY_HPP_

#include "Fx.hpp"

#define FX_DELAY_RANGE 0
#define FX_DELAY_DELAY 1
#define FX_DELAY_DELAYRAND 2
#define FX_DELAY_FEEDBACK 3
#define FX_DELAY_FEEDBACKRAND 4

class FxDelay : public Fx
{
public:
	FxDelay () : FxDelay (nullptr, nullptr, nullptr, nullptr, nullptr) {}

	FxDelay (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, size_t* size) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		sizePtr (size),
		size (1),
		range (1.0f), delay (0.0f), feedback (0.0f) {}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
		size = (sizePtr ? *sizePtr : 1);
		const double r1 = bidist (rnd);
		const double r2 = bidist (rnd);
		range = floor (params ? LIMIT (1.0 + params[SLOTS_OPTPARAMS + FX_DELAY_RANGE] * (NR_STEPS - 1), 1.0, size - 1) : 1.0f);
		delay = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_DELAY_DELAY] + r1 * params[SLOTS_OPTPARAMS + FX_DELAY_DELAYRAND], 0.0, 1.0) : 0.5);
		feedback = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_DELAY_FEEDBACK] + r2 * params[SLOTS_OPTPARAMS + FX_DELAY_FEEDBACKRAND], 0.0, 1.0) : 0.5);
	}

	virtual Stereo play (const double position, const double padsize, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const long frame = framesPerStep * range * delay;
		Stereo s1 = (buffer && (*buffer) ? (**buffer)[frame] : Stereo {0, 0});
		s1 = mix (s0, s1, position, padsize, mixf);
		Stereo s2 = s1;
		if (buffer && (*buffer)) (**buffer)[0] = s2.mix (s0, 1.0f - feedback);
		return s1;
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	size_t* sizePtr;
	size_t size;
	float range;
	float delay;
	float feedback;
};

#endif /* FXDELAY_HPP_ */
