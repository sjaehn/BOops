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

#ifndef FXPHASER_HPP_
#define FXPHASER_HPP_

#include "Fx.hpp"
#include "AllPassFilter.hpp"

#define FX_PHASER_LOFREQ 0
#define FX_PHASER_LOFREQRAND 1
#define FX_PHASER_HIFREQ 2
#define FX_PHASER_HIFREQRAND 3
#define FX_PHASER_MODRATE 4
#define FX_PHASER_MODRATERAND 5
#define FX_PHASER_MODPHASE 6
#define FX_PHASER_MODPHASERAND 7
#define FX_PHASER_FEEDBACK 8
#define FX_PHASER_FEEDBACKRAND 9
#define FX_PHASER_STEPS 10
#define FX_PHASER_MAXSTEPS 10

class FxPhaser : public Fx
{
public:
	FxPhaser () : FxPhaser (nullptr, nullptr, nullptr, nullptr, 24000) {}

	FxPhaser (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, double rate) :
		Fx (buffer, params, pads),
		samplerate (rate),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		loFreq (0.0),
		hiFreq (0.01),
		modRate (0.25),
		modPhase (0.0),
		feedback (0.0f),
		steps (5),
		minDelta (0),
		modDelta (0)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		loFreq = 20.0 + 19980.0 * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_PHASER_LOFREQ] + r1 * params[SLOTS_OPTPARAMS + FX_PHASER_LOFREQRAND], 0.0, 1.0) : 0.0, 3.0);
		const double r2 = bidist (rnd);
		hiFreq = 20.0 + 19980.0 * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_PHASER_HIFREQ] + r2 * params[SLOTS_OPTPARAMS + FX_PHASER_HIFREQRAND], 0.0, 1.0) : 1.0, 3.0);
		const double r3 = bidist (rnd);
		modRate = 10.0 * 2.0 * M_PI * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_PHASER_MODRATE] + r3 * params[SLOTS_OPTPARAMS + FX_PHASER_MODRATERAND], 0.0, 1.0) : 0.1, 3.0);
		const double r4 = bidist (rnd);
		modPhase = 2.0 * M_PI * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_PHASER_MODPHASE] + r4 * params[SLOTS_OPTPARAMS + FX_PHASER_MODPHASERAND], 0.0, 1.0) : 0.0);
		const double r5 = bidist (rnd);
		feedback = 2.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_PHASER_FEEDBACK] + r5 * params[SLOTS_OPTPARAMS + FX_PHASER_FEEDBACKRAND], 0.0, 1.0) : 0.0) - 1.0;
		steps = 1.0 + (params ? LIMIT (10.0 * params[SLOTS_OPTPARAMS + FX_PHASER_STEPS], 0.0, 9.0) : 4.0);

		minDelta = 0.5 * loFreq / samplerate;
		modDelta = (hiFreq > loFreq? 0.5 * hiFreq / samplerate - minDelta : 0.0);
		std::fill (lFilters, lFilters + FX_PHASER_MAXSTEPS, AllPassFilter ());
		std::fill (rFilters, rFilters + FX_PHASER_MAXSTEPS, AllPassFilter ());
		lastSample = Stereo (0, 0);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const double delayL = minDelta + (0.5 - 0.5 * cos (modRate * position * framesPerStep / samplerate)) * modDelta;
		const double delayR = minDelta + (0.5 - 0.5 * cos (modPhase + modRate * position * framesPerStep / samplerate)) * modDelta;
		for (int i = 0; i < steps; ++i)
		{
			lFilters[i].setDelay (delayL);
			rFilters[i].setDelay (delayR);
		}

		Stereo s1 = s0 + lastSample * feedback;
		for (int i = steps - 1; i >= 0; --i)
		{
			s1.left = lFilters[i].process (s1.left);
			s1.right = rFilters[i].process (s1.right);
		}
		lastSample = s1;

		return mix (s0, s1, position, size, mixf);
	}

protected:
	double samplerate;
	double* framesPerStepPtr;
	double framesPerStep;
	double loFreq;
	double hiFreq;
	double modRate;
	double modPhase;
	float feedback;
	int steps;
	AllPassFilter lFilters[FX_PHASER_MAXSTEPS];
	AllPassFilter rFilters[FX_PHASER_MAXSTEPS];
	double minDelta;
	double modDelta;
	Stereo lastSample;
};

#endif /* FXPHASER_HPP_ */
