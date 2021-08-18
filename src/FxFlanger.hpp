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

#ifndef FXFLANGER_HPP_
#define FXFLANGER_HPP_

#include "Fx.hpp"

#define FX_FLANGER_MINDELAY 0
#define FX_FLANGER_MINDELAYRAND 1
#define FX_FLANGER_MODDELAY 2
#define FX_FLANGER_MODDELAYRAND 3
#define FX_FLANGER_FREQ 4
#define FX_FLANGER_FREQRAND 5
#define FX_FLANGER_PHASE 6
#define FX_FLANGER_PHASERAND 7
#define FX_FLANGER_FEEDBACK 8
#define FX_FLANGER_FEEDBACKRAND 9

class FxFlanger : public Fx
{
public:
	FxFlanger () = delete;

	FxFlanger (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, double rate) :
		Fx (buffer, params, pads),
		samplerate (rate),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		minDelay (0.0),
		modDelay (0.01),
		freq (0.25),
		phase (0.0),
		feedback (0.0f)
	{
		if (!framesPerStep) throw std::invalid_argument ("Fx initialized with framesPerStep nullptr");
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		minDelay = 0.01 * LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_MINDELAY] + r1 * params[SLOTS_OPTPARAMS + FX_FLANGER_MINDELAYRAND], 0.0, 1.0);
		const double r2 = bidist (rnd);
		modDelay = 0.01 * LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_MODDELAY] + r2 * params[SLOTS_OPTPARAMS + FX_FLANGER_MODDELAYRAND], 0.0, 1.0);
		const double r3 = bidist (rnd);
		freq = 10.0 * 2.0 * M_PI * pow (LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_FREQ] + r3 * params[SLOTS_OPTPARAMS + FX_FLANGER_FREQRAND], 0.0, 1.0), 3.0);
		const double r4 = bidist (rnd);
		phase = 2.0 * M_PI * LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_PHASE] + r4 * params[SLOTS_OPTPARAMS + FX_FLANGER_PHASERAND], 0.0, 1.0);
		const double r5 = bidist (rnd);
		feedback = 2.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_FEEDBACK] + r5 * params[SLOTS_OPTPARAMS + FX_FLANGER_FEEDBACKRAND], 0.0, 1.0) - 1.0;
		framesPerStep = *framesPerStepPtr;
	}

	virtual Stereo process (const double position, const double size) override
	{
		const double delayL = minDelay + (0.5 - 0.5 * cos (freq * position * framesPerStep / samplerate)) * modDelay;
		const double delayR = minDelay + (0.5 - 0.5 * cos (phase + freq * position * framesPerStep / samplerate)) * modDelay;
		const long frameL = (delayL * samplerate);
		const long frameR = (delayR * samplerate);
		return Stereo ((**buffer)[frameL].left, (**buffer)[frameR].right);
	}

	virtual Stereo playPad (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		Stereo s1 = process (position, size);
		s1 = mix (s0, s1, position, size, mixf);
		Stereo s2 = s1;
		(**buffer).front() = s2.mix (s0, 1.0f - feedback);
		return s1;
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();

		if (shapePaused && (mx >= 0.0001)) init (position);
		shapePaused = (mx < 0.0001);
		if (shapePaused) 
		{
			end();
			return s0;
		}

		Stereo s1 = process (position, size);
		s1 = BUtilities::mix<Stereo> (s0, pan (s0, s1), mx * mixf);
		Stereo s2 = s1;
		(**buffer).front() = s2.mix (s0, 1.0f - feedback);
		return s1;
	}

protected:
	double samplerate;
	double* framesPerStepPtr;
	double framesPerStep;
	double minDelay;
	double modDelay;
	double freq;
	double phase;
	float feedback;
};

#endif /* FXFLANGER_HPP_ */
