/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
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
#include "ACE/ACEReverb.hpp"

#define FX_REVERB_SIZE 0
#define FX_REVERB_SIZERAND 1

class FxReverb : public Fx
{
public:
	FxReverb () = delete;

	FxReverb (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		reverb (rate, 0.75, powf (10.0f, .05f * -20.0f), -0.015f, 1.0f),
		rsize (0.5f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		rsize = 0.5 + 0.5 * LIMIT (params[SLOTS_OPTPARAMS + FX_REVERB_SIZE] + r1 * params[SLOTS_OPTPARAMS + FX_REVERB_SIZERAND], 0.0, 1.0);

		reverb.setRoomSize (rsize);
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo();
		reverb.reverb (&s0.left, &s0.right, &s1.left, &s1.right, 1);
		return s1;
	}

	virtual Stereo playPad (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		const Stereo s1 = process (position, size);
		return mix (s0, s1, position, size, mixf);
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		const Stereo s1 = process (position, size);
		return BUtilities::mix<Stereo> (s0, pan (s0, s1), params[SLOTS_MIX] * mx * mixf);
	}

protected:
	AceReverb reverb;
	float rsize;

};

#endif /* FXREVERB_HPP_ */
