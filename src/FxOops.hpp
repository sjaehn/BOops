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

#ifndef FXOOPSHPP_
#define FXOOPSHPP_

#include "Fx.hpp"
#include "Sample.hpp"

#define FX_OOPSAMP 0
#define FX_OOPSAMPRAND 1
#define FX_OOPSPITCH 2
#define FX_OOPSPITCHRAND 3
#define FX_OOPSOFFSET 4
#define FX_OOPSOFFSETRAND 5

class FxOops : public Fx
{
public:
	FxOops () = delete;

	FxOops (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, double rate, const char* pluginpath) :
		Fx (buffer, params, pads),
		samplerate (rate),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		amp (0.0f), pitch (0.0f), offset (0.0),
		p0 (0.0)
	{
		if (!framesPerStep) throw std::invalid_argument ("Fx initialized with framesPerStep nullptr");

		if (pluginpath)
		{
			char samplepath[1024] = {0};
			strncpy (samplepath, pluginpath, 988);
			strcat (samplepath, "inc/Oops.wav");
			try {oops = Sample (samplepath);}
			catch (std::bad_alloc &ba)
			{
				fprintf (stderr, "BOops.lv2: Can't allocate enoug memory to open inc/Oops.wav.\n");
			}
			catch (std::invalid_argument &ia)
			{
				fprintf (stderr, "%s\n", ia.what());
			}
		}
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		amp = LIMIT (2.0 * (params[SLOTS_OPTPARAMS + FX_OOPSAMP] + r1 * params[SLOTS_OPTPARAMS + FX_OOPSAMPRAND]), 0.0, 2.0);
		const double r2 = bidist (rnd);
		pitch = pow (2.0, LIMIT (2.0 * (params[SLOTS_OPTPARAMS + FX_OOPSPITCH] + r2 * params[SLOTS_OPTPARAMS + FX_OOPSPITCHRAND]) - 1.0, -1.0, 1.0));
		framesPerStep = *framesPerStepPtr;
		offset = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_OOPSOFFSET] + r1 * params[SLOTS_OPTPARAMS + FX_OOPSOFFSETRAND], 0.0, 1.0) : 0.0);
		p0 = position;
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo (0, 0);
		if (position > offset)
		{
			sf_count_t frame = (position - offset - p0) * framesPerStep * pitch;
			s1 = Stereo (oops.get (frame, 0, samplerate), oops.get (frame, 0, samplerate));
		}

		return s0 + s1 * amp;
	}

protected:
	double samplerate;
	double* framesPerStepPtr;
	double framesPerStep;
	Sample oops;
	float amp;
	float pitch;
	double offset;
	double p0;

};

#endif /* FXOOPSHPP_ */
