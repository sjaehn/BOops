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

#ifndef FX_HPP_
#define FX_HPP_

#ifndef LIMIT
#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))
#endif /* LIMIT */

#include <random>
#include <ctime>
#include "Stereo.hpp"
#include "RingBuffer.hpp"
#include "Pad.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"

class Fx
{
public:
	Fx () : Fx (nullptr, nullptr, nullptr) {}

	Fx (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		buffer (buffer), params (params), pads (pads), playing (false), startPos (0),
		panf {1.0f, 1.0f}, unpanf {0.0f, 0.0f}, rnd (time (0)), unidist (0.0, 1.0), bidist (-1.0, 1.0)
	{}

	virtual ~Fx () {};

	// DO NOT OVERRIDE DEFAULT COPY CONSTRUCTOR AND ASSIGNMENT OPERATOR
	// This class is not responsible for the imported pointers
	// buffer, params, and pads!

	int getStart () const {return startPos;}

	int getStart (const double position) const
	{
		if (!pads) return -1;

		for (int i = position; i >= 0; --i)
		{
			if ((pads[i].gate > 0) && (pads[i].mix > 0)) return i;
		}

		return -1;
	}

	bool isPad (const double position) const
	{
		if (!pads) return false;

		int start = getStart (position);
		return ((start >= 0) && (start + pads[start].size > position));
	}

	virtual void start (const double position)
	{
		startPos = getStart (position);
		playing = (isPad (position) && (unidist (rnd) < (pads ? pads[startPos >= 0 ? startPos : 0].gate : 0)));
		panf = (Stereo {1.0, 1.0}).pan ((params ? params[SLOTS_PAN] : 0.0));
		unpanf = Stereo {1.0, 1.0} - panf;
	}

	virtual Stereo play (const double position) {return (buffer && (*buffer) ? (**buffer)[0] : Stereo {0, 0});}

	virtual void end () {playing = false;}

protected:
	RingBuffer<Stereo>** buffer;
	float* params;
	Pad* pads;
	bool playing;
	int startPos;
	Stereo panf;
	Stereo unpanf;
	std::minstd_rand rnd;
	std::uniform_real_distribution<float> unidist;
	std::uniform_real_distribution<float> bidist;

	float adsr (const double position)
	{
		if ((!pads) || (!params)) return 0;

		float adr = params[SLOTS_ATTACK] + params[SLOTS_DECAY] + params[SLOTS_RELEASE];
		if (adr < 1.0f) adr = 1.0f;
		double padStart = getStart();
		double padSize = pads[int (padStart)].size;

		if ((position < padStart) || (position >= padStart + padSize)) return 0;

		if (position < padStart + params[SLOTS_ATTACK] / adr) return (position - padStart) / (params[SLOTS_ATTACK] / adr);

		if (position < padStart + params[SLOTS_DECAY] / adr) return
		(
			1.0f -
			(1.0f - params[SLOTS_SUSTAIN]) *
			(position - padStart - (params[SLOTS_ATTACK] / adr)) /
			((params[SLOTS_DECAY] - params[SLOTS_ATTACK]) / adr)
		);

		if (position > padStart + padSize - params[SLOTS_RELEASE] / adr) return params[SLOTS_SUSTAIN] * (padStart + padSize - position) / (params[SLOTS_RELEASE] / adr);

		return params[SLOTS_SUSTAIN];
	}

	Stereo mix (const Stereo& dry, const Stereo& wet, double position)
	{
		Stereo s1 = panf * wet;
		const Stereo s2 = unpanf * dry;
		s1 = s1 + s2;
		s1.mix (dry, 1.0f - pads[startPos].mix);
		return s1.mix (dry, 1.0f - params[SLOTS_MIX] * adsr (position));
	}
};

#endif /* FX_HPP_ */
