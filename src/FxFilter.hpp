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

#ifndef FXFILTER_HPP_
#define FXFILTER_HPP_

#include "Fx.hpp"
#include "ButterworthBandPassFilter.hpp"

#define FX_FILTER_LOW 0
#define FX_FILTER_LOWRAND 1
#define FX_FILTER_HIGH 2
#define FX_FILTER_HIGHRAND 3
#define FX_FILTER_ORDER 4

class BNoname01; // Forward declaration

class FxFilter : public Fx
{
public:
	FxFilter () : FxFilter (nullptr, nullptr, nullptr, 0) {}
	
	FxFilter (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		rate (rate), filter (48000, 20, 20000, 8) {}

	virtual void start (const double position) override
	{
		Fx::start (position);
		const double r1 = bidist (rnd);
		double low = 20.0f + 19980.0f * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FILTER_LOW] + r1 * params[SLOTS_OPTPARAMS + FX_FILTER_LOWRAND], 0.0, 1.0) : 0.0, 4.0);
		const double r2 = bidist (rnd);
		double high = 20.0f + 19980.0f * pow (params ? LIMIT (params[SLOTS_OPTPARAMS + FX_FILTER_HIGH] + r2 * params[SLOTS_OPTPARAMS + FX_FILTER_HIGHRAND], 0.0, 1.0) : 1.0, 4.0);
		int order = 2 * int (params ? LIMIT (1.0 + 8.0 * params[SLOTS_OPTPARAMS + FX_FILTER_ORDER], 1.0, 8.0) : 4.0);
		filter = ButterworthBandPassFilter (rate, low, high, order);
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		Stereo s1 = s0;
		s1 = filter.push (s1);
		return mix (s0, s1, position);
	}

protected:
	double rate;
	ButterworthBandPassFilter filter;
};

#endif /* FXFILTER_HPP_ */
