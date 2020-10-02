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

#ifndef FXAMP_HPP_
#define FXAMP_HPP_

#include "Fx.hpp"

#define FX_AMP_AMP 0
#define FX_AMP_AMPRAND 1

class FxAmp : public Fx
{
public:
	FxAmp () : FxAmp (nullptr, nullptr, nullptr) {}

	FxAmp (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		amp (0.0f)
	{}

	virtual void start (const double position) override
	{
		Fx::start (position);
		amp =
		(
			params ?
			LIMIT
			(
				2.0 * (params[SLOTS_OPTPARAMS + FX_AMP_AMP] + bidist (rnd) * params[SLOTS_OPTPARAMS + FX_AMP_AMPRAND]),
				0.0,
				2.0
			) :
			1.0
		);
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		Stereo s1 = s0 * amp;
		s1.mix (s0, 1.0f - pads[startPos].mix);
		return s1.mix (s0, 1.0f - params[SLOTS_MIX] * adsr (position));
	}

protected:
	float amp;

};

#endif /* FXAMP_HPP_ */
