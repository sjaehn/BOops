/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * FxInfinity
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

#ifndef FXINFINITY_HPP_
#define FXINFINITY_HPP_

#include "Fx.hpp"
#include "Airwindows/Infinity2.hpp"

#define FX_INFINITY_FILTER 0
#define FX_INFINITY_FILTERRAND 1
#define FX_INFINITY_SIZE 2
#define FX_INFINITY_SIZERAND 3
#define FX_INFINITY_DAMP 4
#define FX_INFINITY_DAMPRAND 5
#define FX_INFINITY_ALLPASS 6
#define FX_INFINITY_ALLPASSRAND 7
#define FX_INFINITY_FEEDBACK 8
#define FX_INFINITY_FEEDBACKRAND 9

class FxInfinity : public Fx
{
public:
	FxInfinity () = delete;

	FxInfinity (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		infinity (rate, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
		filter (1.0f),
		rsize (0.5f),
		damp (0.0f),
		allpass (1.0f),
		feedback (1.0f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		filter = LIMIT (params[SLOTS_OPTPARAMS + FX_INFINITY_FILTER] + r1 * params[SLOTS_OPTPARAMS + FX_INFINITY_FILTERRAND], 0.0, 1.0);
		const double r2 = bidist (rnd);
		rsize = LIMIT (params[SLOTS_OPTPARAMS + FX_INFINITY_SIZE] + r2 * params[SLOTS_OPTPARAMS + FX_INFINITY_SIZERAND], 0.0, 1.0);
		const double r3 = bidist (rnd);
		damp = LIMIT (params[SLOTS_OPTPARAMS + FX_INFINITY_DAMP] + r3 * params[SLOTS_OPTPARAMS + FX_INFINITY_DAMPRAND], 0.0, 1.0);
		const double r4 = bidist (rnd);
		allpass = LIMIT (params[SLOTS_OPTPARAMS + FX_INFINITY_ALLPASS] + r4 * params[SLOTS_OPTPARAMS + FX_INFINITY_ALLPASSRAND], 0.0, 1.0);
		const double r5 = bidist (rnd);
		feedback = LIMIT (params[SLOTS_OPTPARAMS + FX_INFINITY_FEEDBACK] + r5 * params[SLOTS_OPTPARAMS + FX_INFINITY_FEEDBACKRAND], 0.0, 1.0);

		infinity.setParameter (0, filter);
		infinity.setParameter (1, rsize);
		infinity.setParameter (2, damp);
		infinity.setParameter (3, allpass);
		infinity.setParameter (4, feedback);
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo();
		infinity.process (&s0.left, &s0.right, &s1.left, &s1.right, 1);
		return s1;
	}

	virtual Stereo playPad (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		const Stereo s1 = process (position, size);
		if (!playing) return s0;

		return mix (s0, s1, position, size, mixf);
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		const Stereo s1 = process (position, size);

		if (shapePaused && (mx >= 0.0001)) init (position);
		shapePaused = (mx < 0.0001);

		return BUtilities::mix<Stereo> (s0, pan (s0, s1), mx * mixf);
	}

protected:
	Infinity2 infinity;
	float filter;
	float rsize;
	float damp;
	float allpass;
	float feedback;

};

#endif /* FXINFINITY_HPP_ */
