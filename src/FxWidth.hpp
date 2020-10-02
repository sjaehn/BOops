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

#ifndef FXWIDTH_HPP_
#define FXWIDTH_HPP_

#include "Fx.hpp"

#define FX_WIDTH_WIDTH 0
#define FX_WIDTH_WIDTHRAND 1

class FxWidth : public Fx
{
public:
	FxWidth () : FxWidth (nullptr, nullptr, nullptr) {}

	FxWidth (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		width (0.0f)
	{}

	virtual void start (const double position) override
	{
		Fx::start (position);
		width =
		(
			params ?
			pow
			(
				2 * LIMIT (params[SLOTS_OPTPARAMS + FX_WIDTH_WIDTH] + bidist (rnd) * params[SLOTS_OPTPARAMS + FX_WIDTH_WIDTHRAND], 0.0, 1.0),
				6.64385619
			) : 
			1.0
		);
	}

	virtual Stereo play (const double position) override
	{
		const Stereo s0 = (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});
		if ((!playing) || (!pads) || (startPos < 0) || (!pads[startPos].mix) || (position < double (startPos)) || (position > double (startPos) + pads[startPos].size)) return s0;

		const float m = (s0.left + s0.right) / 2;
		const float x = (s0.left - s0.right) * width / 2;

		Stereo s1 = Stereo {m + x, m - x};
		s1.mix (s0, 1.0f - pads[startPos].mix);
		return s1.mix (s0, 1.0f - params[SLOTS_MIX] * adsr (position));
	}

protected:
	float width;

};

#endif /* FXWIDTH_HPP_ */
