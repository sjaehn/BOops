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

#ifndef FXCHOPPER_HPP_
#define FXCHOPPER_HPP_

#include "Fx.hpp"

#define FX_CHOPPER_NR 0
#define FX_CHOPPER_SMOOTH 1
#define FX_CHOPPER_STEPS 2
#define FX_CHOPPER_STEPRAND 10

class FxChopper : public Fx
{
public:
	FxChopper () : FxChopper (nullptr, nullptr, nullptr) {}

	FxChopper (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		nr (0), smoothing (0.1f)
	{}

	virtual void start (const double position) override
	{
		Fx::start (position);
		if (params)
		{
			nr = LIMIT (1 + 8.0f * params[SLOTS_OPTPARAMS + FX_CHOPPER_NR], 1, 8);
			smoothing = params[SLOTS_OPTPARAMS + FX_CHOPPER_SMOOTH];
			for (int i = 0; i < nr; ++i)
			{
				const double r = bidist (rnd);
				chops[i] = LIMIT
				(
					params[SLOTS_OPTPARAMS + FX_CHOPPER_STEPS + i] + r * params[SLOTS_OPTPARAMS + FX_CHOPPER_STEPRAND],
					0.0,
					1.0
				);
			}
		}
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		const int p = nr * fmod (position, 1.0);
		const double frac = double (nr) * fmod (position, 1.0) - double (p);
		const int step = p % nr;
		Stereo s1 = s0 * chops[step];

		if (frac < 0.5 * smoothing)
		{
			const int prev = (p + nr - 1) % nr;
			const Stereo sp = s0 * chops[prev];
			s1.mix (sp, 0.5 - frac / smoothing);
		}

		else if (frac > 1.0 - 0.5 * smoothing)
		{
			const int next = (p + 1) % nr;
			const Stereo sn = s0 * chops[next];
			s1.mix (sn, 0.5 - (1.0 - frac) / smoothing);
		}

		s1.mix (s0, 1.0f - pads[startPos].mix);
		return s1.mix (s0, 1.0f - params[SLOTS_MIX] * adsr (position));
	}

protected:
	int nr;
	float smoothing;
	float chops[8];

};

#endif /* FXCHOPPER_HPP_ */
