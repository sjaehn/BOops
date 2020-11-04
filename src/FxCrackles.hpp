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

#ifndef FXCRACKLES_HPP_
#define FXCRACKLES_HPP_

#include "Fx.hpp"
#include "Crackle.hpp"
#include "StaticArrayList.hpp"

#ifndef DB2CO
#define DB2CO(x) pow (10, 0.05 * (x))
#endif

#define FX_CRACKLES_AMP 0
#define FX_CRACKLES_AMPRAND 1
#define FX_CRACKLES_RATE 2
#define FX_CRACKLES_RATERAND 3
#define FX_CRACKLES_MAXSIZE 4
#define FX_CRACKLES_MAXSIZERAND 5
#define FX_CRACKLES_DISTRIBUTION 6
#define FX_CRACKLES_DISTRIBUTIONRAND 7
#define MAXCRACKLES 16
#define CRACKLEFREQ 10000
#define CRACKLEFREQRAND 2000

class FxCrackles : public Fx
{
public:
	FxCrackles () : FxCrackles (nullptr, nullptr, nullptr, nullptr, 48000) {}

	FxCrackles (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, const double samplerate) :
		Fx (buffer, params, pads),
		samplerate (samplerate),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		rate (0.0f),
		c (0)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
		const double r1 = bidist (rnd);
		const float db = -36.0 + 48.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_CRACKLES_AMP] + r1 * params[SLOTS_OPTPARAMS + FX_CRACKLES_AMPRAND], 0.0, 1.0) : 0.5);
		const float amp = DB2CO (db);
		const double r2 = bidist (rnd);
		rate = 200.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_CRACKLES_RATE] + r2 * params[SLOTS_OPTPARAMS + FX_CRACKLES_RATERAND], 0.0, 1.0) : 0.5);
		const double r3 = bidist (rnd);
		maxsize = amp * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_CRACKLES_MAXSIZE] + r3 * params[SLOTS_OPTPARAMS + FX_CRACKLES_MAXSIZERAND], 0.0, 1.0) : 0.5);
		const double r4 = bidist (rnd);
		distrib = 10.0 * (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_CRACKLES_DISTRIBUTION] + r4 * params[SLOTS_OPTPARAMS + FX_CRACKLES_DISTRIBUTIONRAND], 0.0, 1.0) : 0.5);
		c = 0;
		crackles.clear();
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		double t = double (c) / samplerate;
		++c;

		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		// Randomly generate new crackles
		const double r = unidist (rnd);
		if (r < rate / framesPerStep)
		{
			const double r1 = unidist (rnd);
			const double r2 = bidist (rnd);
			const double level = maxsize * pow (r1, distrib);
			const double freq = CRACKLEFREQ + r2 * CRACKLEFREQRAND;
			crackles.push_back (Crackle (t, level, freq));
		}

		// Iterate through crackles
		Stereo s1 = s0;
		for (Crackle** iit = crackles.begin(); iit < crackles.end ();)
		{
			double cr = (**iit).get (t);
			s1 +=  Stereo (cr, cr);

			// Delete if < -70 db
			if ((**iit).getAmplitude (t) < 0.000316228) iit = crackles.erase (iit);
			else ++iit;
		}

		return mix (s0, s1, position, size, mixf);
	}

protected:
	double samplerate;
	double* framesPerStepPtr;
	double framesPerStep;
	float rate;
	float maxsize;
	float distrib;
	StaticArrayList<Crackle, MAXCRACKLES> crackles;
	long c;

};

#endif /* FXCRACKLES_HPP_ */
