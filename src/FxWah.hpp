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

#ifndef FXWAH_HPP_
#define FXWAH_HPP_

#include "Fx.hpp"
#include "ButterworthBandPassFilter.hpp"
#include "Shape.hpp"

#define FX_WAH_CFREQ 0
#define FX_WAH_CFREQRAND 1
#define FX_WAH_DEPTH 2
#define FX_WAH_DEPTHRAND 3
#define FX_WAH_WIDTH 4
#define FX_WAH_WIDTHRAND 5
#define FX_WAH_ORDER 6

class FxWah : public Fx
{
public:
	FxWah () : FxWah (nullptr, nullptr, nullptr, 24000, nullptr) {}

	FxWah (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate, Shape<SHAPE_MAXNODES>* shape) :
		Fx (buffer, params, pads),
		rate (rate),
		shape (shape),
		cFreq (500.0f),
		depth (0.5f),
		width (0.1f),
		order (2),
		filter (48000, 20, 20000, 8)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r3 = bidist (rnd);
		cFreq = 20.0 + 19980.0 * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_WAH_CFREQ] + r3 * params[SLOTS_OPTPARAMS + FX_WAH_CFREQRAND], 0.0, 1.0) : 0.5, 3.0);
		const double r4 = bidist (rnd);
		depth = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_WAH_DEPTH] + r4 * params[SLOTS_OPTPARAMS + FX_WAH_DEPTHRAND], 0.0, 1.0) : 0.5);
		const double r5 = bidist (rnd);
		width = (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_WAH_WIDTH] + r5 * params[SLOTS_OPTPARAMS + FX_WAH_WIDTHRAND], 0.0, 1.0) : 0.1);
		order = 2 * int (params ? LIMIT (1.0 + 8.0 * params[SLOTS_OPTPARAMS + FX_WAH_ORDER], 0, 8) : 2);

		const float m = (shape ? shape->getMapValue (0): 0.0);
		const float f = cFreq * (1 + depth * m);
		filter = ButterworthBandPassFilter (rate, f * (1.0 - 0.5 * width), f * (1.0 + 0.5 * width), order);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads)) return s0;

		const float m = (shape ? shape->getMapValue (fmod (position, 1.0)): 0.0);
		const float f = cFreq * (1 + depth * m);
		const float fmin = LIMIT (f * (1.0 - width), 0.0, 20000.0);
		const float fmax = LIMIT (f * (1.0 + width), 0.0, 20000.0);
		filter.set (rate, fmin, fmax, order);
		const Stereo s1 = filter.push (s0);

		return mix (s0, s1, position, size, mixf);
	}

protected:
	double rate;
	Shape<SHAPE_MAXNODES>* shape;
	float cFreq;
	float depth;
	float width;
	int order;
	ButterworthBandPassFilter filter;
};

#endif /* FXWAH_HPP_ */
