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

#ifndef FXSTUTTER_HPP_
#define FXSTUTTER_HPP_

#include "Fx.hpp"

#define FX_STUTTER_STUTTERS 0
#define FX_STUTTER_SMOOTH 1

class FxStutter : public Fx
{
public:
	FxStutter () : FxStutter (nullptr, nullptr, nullptr, nullptr) {}

	FxStutter (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep), framesPerStep (24000), framesPerStutter (24000),
		stutters (1), smoothing (0.1f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = (framesPerStepPtr ? *framesPerStepPtr : 24000.0);
		smoothing = (params ? params[SLOTS_OPTPARAMS + FX_STUTTER_SMOOTH] : 0.1f);
		stutters = (params ? LIMIT (2.0 + 7.0 * params[SLOTS_OPTPARAMS + FX_STUTTER_STUTTERS], 2, 8) : 4);
		framesPerStutter = framesPerStep / double (stutters);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const long nr = position * double (stutters);
		const double frac = fmod (position, 1.0 / double (stutters));
		const long frame = nr * framesPerStutter;
		Stereo s1 = (buffer && (*buffer) ? (**buffer)[frame] : Stereo {0, 0});

		if (frac < 0.5 * smoothing)
		{
			const long f2 = (nr > 0 ? (nr - 1) * framesPerStutter : nr * framesPerStutter);
			Stereo s2 = (buffer && (*buffer) ? (**buffer)[f2] : Stereo {0, 0});
			s1.mix (s2, 0.5 - frac / smoothing);
		}

		else if (frac > 1.0 - 0.5 * smoothing)
		{
			const long f2 = (nr + 1) * framesPerStutter;
			Stereo s2 = (buffer && (*buffer) ? (**buffer)[f2] : Stereo {0, 0});
			s1.mix (s2, 0.5 - (1.0 - frac) / smoothing);
		}

		return mix (s0, s1, position, size, mixf);
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	double framesPerStutter;
	int stutters;
	float smoothing;

};

#endif /* FXSTUTTER_HPP_ */
