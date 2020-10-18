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
	FxFlanger () : FxFlanger (nullptr, nullptr, nullptr, nullptr, 24000) {}

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
	{}

	virtual void start (const double position) override
	{
		Fx::start (position);
		const double r1 = bidist (rnd);
		minDelay = 0.01 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_MINDELAY] + r1 * params[SLOTS_OPTPARAMS + FX_FLANGER_MINDELAYRAND], 0.0, 1.0) : 0.0);
		const double r2 = bidist (rnd);
		modDelay = 0.01 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_MODDELAY] + r2 * params[SLOTS_OPTPARAMS + FX_FLANGER_MODDELAYRAND], 0.0, 1.0) : 0.3333);
		const double r3 = bidist (rnd);
		freq = 10.0 * 2.0 * M_PI * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_FREQ] + r3 * params[SLOTS_OPTPARAMS + FX_FLANGER_FREQRAND], 0.0, 1.0) : 0.1, 2.0);
		const double r4 = bidist (rnd);
		phase = 2.0 * M_PI * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_PHASE] + r4 * params[SLOTS_OPTPARAMS + FX_FLANGER_PHASERAND], 0.0, 1.0) : 0.0);
		const double r5 = bidist (rnd);
		feedback = 2.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FLANGER_FEEDBACK] + r5 * params[SLOTS_OPTPARAMS + FX_FLANGER_FEEDBACKRAND], 0.0, 1.0) : 0.0) - 1.0;
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		const double delayL = minDelay + (0.5 - 0.5 * cos (freq * (position - startPos) * framesPerStep / samplerate)) * modDelay;
		const double delayR = minDelay + (0.5 - 0.5 * cos (phase + freq * (position - startPos) * framesPerStep / samplerate)) * modDelay;
		const long frameL = (delayL * samplerate);
		const long frameR = (delayR * samplerate);
		Stereo s1 = (buffer && (*buffer) ? Stereo ((**buffer)[frameL].left, (**buffer)[frameR].right) : Stereo {0, 0});
		s1 = mix (s0, s1, position);
		Stereo s2 = s1;
		if (buffer && (*buffer)) (**buffer)[0] = s2.mix (s0, 1.0f - feedback);
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
