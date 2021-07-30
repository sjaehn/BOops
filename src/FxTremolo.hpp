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

#ifndef FXTREMOLO_HPP_
#define FXTREMOLO_HPP_

#include "Fx.hpp"

#define FX_TREMOLO_RATE 0
#define FX_TREMOLO_RATERAND 1
#define FX_TREMOLO_DEPTH 2
#define FX_TREMOLO_DEPTHRAND 3
#define FX_TREMOLO_ENV 4

class FxTremolo : public Fx
{
public:
	FxTremolo () = delete;

	FxTremolo (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, double rate) :
		Fx (buffer, params, pads),
		samplerate (rate),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		freq (10.0), depth (0.5),
		env (SINE_WAVE),
		eval (0.0f) {}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		freq = 1.0 + 19.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_TREMOLO_RATE] + r1 * params[SLOTS_OPTPARAMS + FX_TREMOLO_RATERAND], 0.0, 1.0);
		const double r2 = bidist (rnd);
		depth = LIMIT (params[SLOTS_OPTPARAMS + FX_TREMOLO_DEPTH] + r2 * params[SLOTS_OPTPARAMS + FX_TREMOLO_DEPTHRAND], 0.0, 1.0);
		env = BOopsWaveformIndex (LIMIT (int (round (params[SLOTS_OPTPARAMS + FX_RINGMOD_ENV] * 8)), 0, 4));

		framesPerStep = *framesPerStepPtr;
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		float f = 0.0f;
		double t = position * framesPerStep / samplerate;
		switch (env)
		{
			case SINE_WAVE:			f = sin (2.0 * M_PI * t * freq);
									break;

			case TRIANGLE_WAVE:		f = (fmod (t * freq, 1.0) < 0.5 ? 4.0 * fmod (t * freq, 1.0) - 1.0 : 3.0 - 4.0 * fmod (t * freq, 1.0));
									break;

			case SQUARE_WAVE:		f = (fmod (t * freq, 1.0) < 0.5 ? 1.0 : -1.0);
									break;

			case SAW_WAVE:			f = 2.0 * fmod (t * freq, 1.0) - 1.0;
									break;

			case REVERSE_SAW_WAVE:	f = 1.0 - 2.0 * fmod (t * freq, 1.0);
									break;
		};
		eval += LIMIT (f - eval, -1.0f / (0.005f * samplerate), 1.0f / (0.005f * samplerate));
		return s0 * (1.0f - depth * 0.5f * (eval + 1.0f));
	}

protected:
	double samplerate;
	double* framesPerStepPtr;
	double framesPerStep;
	float freq;
	float depth;
	BOopsWaveformIndex env;
	float eval;
};

#endif /* FXTREMOLO_HPP_ */
