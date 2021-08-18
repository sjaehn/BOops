/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * FxGalactic
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

#ifndef FXGALACTIC_HPP_
#define FXGALACTIC_HPP_

#include "Fx.hpp"
#include "Airwindows/Galactic.hpp"

#define FX_GALACTIC_REPLACE 0
#define FX_GALACTIC_REPLACERAND 1
#define FX_GALACTIC_BRIGHTNESS 2
#define FX_GALACTIC_BRIGHTNESSRAND 3
#define FX_GALACTIC_DETUNE 4
#define FX_GALACTIC_DETUNERAND 5
#define FX_GALACTIC_BIGNESS 6
#define FX_GALACTIC_BIGNESSRAND 7

class FxGalactic : public Fx
{
public:
	FxGalactic () = delete;

	FxGalactic (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		galactic (rate, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f),
		replace (0.5f),
		brightness (0.5f),
		detune (0.5f),
		bigness (0.5f)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		replace = LIMIT (params[SLOTS_OPTPARAMS + FX_GALACTIC_REPLACE] + r1 * params[SLOTS_OPTPARAMS + FX_GALACTIC_REPLACERAND], 0.0, 1.0);
		const double r2 = bidist (rnd);
		brightness = LIMIT (params[SLOTS_OPTPARAMS + FX_GALACTIC_BRIGHTNESS] + r2 * params[SLOTS_OPTPARAMS + FX_GALACTIC_BRIGHTNESSRAND], 0.0, 1.0);
		const double r3 = bidist (rnd);
		detune = LIMIT (params[SLOTS_OPTPARAMS + FX_GALACTIC_DETUNE] + r3 * params[SLOTS_OPTPARAMS + FX_GALACTIC_DETUNERAND], 0.0, 1.0);
		const double r4 = bidist (rnd);
		bigness = LIMIT (params[SLOTS_OPTPARAMS + FX_GALACTIC_BIGNESS] + r4 * params[SLOTS_OPTPARAMS + FX_GALACTIC_BIGNESSRAND], 0.0, 1.0);

		galactic.setParameter (0, replace);
		galactic.setParameter (1, brightness);
		galactic.setParameter (2, detune);
		galactic.setParameter (3, bigness);
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo();
		galactic.process (&s0.left, &s0.right, &s1.left, &s1.right, 1);
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
		if (shapePaused) 
		{
			end();
			return s0;
		}

		return BUtilities::mix<Stereo> (s0, pan (s0, s1), mx * mixf);
	}

protected:
	Galactic galactic;
	float replace;
	float brightness;
	float detune;
	float bigness;

};

#endif /* FXGALACTIC_HPP_ */
