/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * FxReverb
 * Reverb effect based on a-reverb <https://github.com/Ardour/ardour/tree/master/libs/plugins/a-reverb.lv2>
 *
 * Copyright (C) 2003-2004 Fredrik Kilander <fk@dsv.su.se>
 * Copyright (C) 2008-2016 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2012 Will Panther <pantherb@setbfree.org>
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2020 - 2021 by Sven Jähnichen
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

#ifndef FXREVERB_HPP_
#define FXREVERB_HPP_

#include "Fx.hpp"
#include "ACEReverb.hpp"

#define FX_REVERB_SIZE 0
#define FX_REVERB_SIZERAND 1

class FxReverb : public Fx
{
public:
	FxReverb () = delete;

	FxReverb (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		reverb (rate, 0.75, powf (10.0f, .05f * -20.0f), -0.015f, 0.5f),
		rsize (0.5f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r2 = bidist (rnd);
		rsize = 0.5 + 0.5 * LIMIT (params[SLOTS_OPTPARAMS + FX_REVERB_SIZE] + r2 * params[SLOTS_OPTPARAMS + FX_REVERB_SIZERAND], 0.0, 1.0);

		reverb.clear();
		reverb.setRoomSize (rsize);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		Stereo s1 = Stereo();
		reverb.reverb (&s0.left, &s0.right, &s1.left, &s1.right, 1);
		return mix (s0, s1, position, size, mixf);
	}

protected:
	AceReverb reverb;
	float rsize;

};

#endif /* FXREVERB_HPP_ */
