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
	FxDelay () = delete;

	FxDelay (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, size_t* size) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		sizePtr (size),
		size (1),
		range (1.0f), delay (0.0f), feedback (0.0f)
	{
		if (!framesPerStep) throw std::invalid_argument ("Fx initialized with framesPerStep nullptr");
		if (!size) throw std::invalid_argument ("Fx initialized with size nullptr");
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = *framesPerStepPtr;
		size = *sizePtr;
		const double r1 = bidist (rnd);
		const double r2 = bidist (rnd);
		range = floor (LIMIT (1.0 + params[SLOTS_OPTPARAMS + FX_DELAY_RANGE] * NR_STEPS, 1.0, *sizePtr - 1));
		delay = LIMIT (params[SLOTS_OPTPARAMS + FX_DELAY_DELAY] + r1 * params[SLOTS_OPTPARAMS + FX_DELAY_DELAYRAND], 0.0, 1.0);
		feedback = LIMIT (params[SLOTS_OPTPARAMS + FX_DELAY_FEEDBACK] + r2 * params[SLOTS_OPTPARAMS + FX_DELAY_FEEDBACKRAND], 0.0, 1.0);
	}

	virtual Stereo process (const double position, const double size) override
	{
		return getSample (*framesPerStepPtr * range * delay);
	}

	virtual Stereo playPad (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = process (position, size);
		s1 = mix (s0, s1, position, size, mixf);
		Stereo s2 = s1;
		(**buffer).front() = s2.mix (s0, 1.0f - feedback);
		return s1;
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = process (position, size);
		s1 = BUtilities::mix<Stereo> (s0, pan (s0, s1), params[SLOTS_MIX] * mx * mixf);
		Stereo s2 = s1;
		(**buffer).front() = s2.mix (s0, 1.0f - feedback);
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
