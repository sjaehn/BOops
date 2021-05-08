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

#ifndef FXJUMBLER_HPP_
#define FXJUMBLER_HPP_

#include "Fx.hpp"

class FxJumbler : public Fx
{
public:
	FxJumbler () = delete;

	FxJumbler (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double* framesPerStep, size_t* size) :
		Fx (buffer, params, pads),
		framesPerStepPtr (framesPerStep),
		framesPerStep (24000),
		sizePtr (size),
		size (1),
		delay (0)
	{
		if (!framesPerStep) throw std::invalid_argument ("Fx initialized with framesPerStep nullptr");
		if (!size) throw std::invalid_argument ("Fx initialized with size nullptr");
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		framesPerStep = *framesPerStepPtr;
		size = *sizePtr;

		delay = 0;

		// Count number of blocks
		int blocks = 0;
		for (size_t i = 0; i < size; ++i)
		{
			if ((pads[i].gate != 0) && (pads[i].size != 0) && (pads[i].mix != 0)) ++blocks;
		}

		const int r1 = unidist (rnd) * double (blocks);
		const int dblock = LIMIT (r1, 0, blocks - 1);

		// Calculate delay
		blocks = 0;
		for (size_t i = 0; i < size; ++i)
		{
			if ((pads[i].gate != 0) && (pads[i].size != 0) && (pads[i].mix != 0)) ++blocks;
			if (blocks >= dblock)
			{
				delay = (int (position) + size - i) % size;
				break;
			}
		}
	}

	virtual Stereo play (const double position, const double padsize, const double mixf) override
	{
		const Stereo s0 = (**buffer)[0];
		if (!playing) return s0;

		const long frame = framesPerStep * delay;
		const Stereo s1 = (**buffer)[frame];
		return mix (s0, s1, position, padsize, mixf);
	}

protected:
	double* framesPerStepPtr;
	double framesPerStep;
	size_t* sizePtr;
	size_t size;
	int delay;
};

#endif /* FXJUMBLER_HPP_ */
