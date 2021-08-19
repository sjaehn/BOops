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

#ifndef FX_HPP_
#define FX_HPP_

#ifndef LIMIT
#define LIMIT(g , min, max) ((g) > (max) ? (max) : ((g) < (min) ? (min) : (g)))
#endif /* LIMIT */

#include <random>
#include <ctime>
#include <stdexcept>
#include "Stereo.hpp"
#include "RingBuffer.hpp"
#include "Pad.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Shape.hpp"

class Fx
{
public:
	Fx () = delete;

	Fx (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		buffer (buffer), params (params), pads (pads),
		shapePaused (true), playing (false), panf (), unpanf(),
		rnd (time (0)), unidist (0.0, 1.0), bidist (-1.0, 1.0)
	{
		if (!buffer) throw std::invalid_argument ("Fx initialized with buffer nullptr");
		if (!params) throw std::invalid_argument ("Fx initialized with parameters nullptr");
		if (!pads) throw std::invalid_argument ("Fx initialized with pads nullptr");
	}

	virtual ~Fx ()
	{};

	// DO NOT OVERRIDE DEFAULT COPY CONSTRUCTOR AND ASSIGNMENT OPERATOR
	// This class is not responsible for the imported pointers
	// buffer, params, and pads!

	virtual void init (const double position)
	{
		const int startPos = position;
		playing = (unidist (rnd) < pads[startPos >= 0 ? startPos : 0].gate);
		panf = (Stereo {1.0, 1.0}).pan (params[SLOTS_PAN]);
		unpanf = Stereo {1.0, 1.0} - panf;
	}

	virtual Stereo process (const double position, const double size)
	{
		return (**buffer).front();
	}

	virtual Stereo playPad (const double position, const double size, const double mixf)
	{
		return mix ((**buffer).front(), process (position, size), position, size, mixf);
	}

	virtual Stereo play (const double position, const double size, const double mx, const double mixf)
	{
		return BUtilities::mix<Stereo> ((**buffer).front(), pan ((**buffer).front(), process (position, size)), params[SLOTS_MIX] * mx * mixf);
	}

	virtual void end () {playing = false;}

	virtual bool isPlaying () {return playing;}

protected:
	RingBuffer<Stereo>** buffer;
	float* params;
	Pad* pads;
	bool shapePaused;
	bool playing;
	Stereo panf;
	Stereo unpanf;
	std::minstd_rand rnd;
	std::uniform_real_distribution<float> unidist;
	std::uniform_real_distribution<float> bidist;

	float adsr (const double position, const double size) const
	{
		if ((position < 0) || (position >= size)) return 0;

		float adr = params[SLOTS_ATTACK] + params[SLOTS_DECAY] + params[SLOTS_RELEASE];

		if (adr < 1.0f) adr = 1.0f;

		if (position < params[SLOTS_ATTACK] / adr) return position / (params[SLOTS_ATTACK] / adr);

		if (position < (params[SLOTS_ATTACK] + params[SLOTS_DECAY]) / adr) return
		(
			1.0f -
			(1.0f - params[SLOTS_SUSTAIN]) *
			(position - (params[SLOTS_ATTACK] / adr)) /
			(params[SLOTS_DECAY] / adr)
		);

		if (position > size - params[SLOTS_RELEASE] / adr) return params[SLOTS_SUSTAIN] * (size - position) / (params[SLOTS_RELEASE] / adr);

		return params[SLOTS_SUSTAIN];
	}

	Stereo getSample (const double frame)
	{
		const float x = fmodf (frame, 1.0);
		if (x == 0.0f) return (**buffer)[frame];
		if (frame < 1.0) return BUtilities::mix<Stereo> ((**buffer)[frame], (**buffer)[frame + 1], x);
		
		// 4-point, 3rd-order Hermite (x-form)
		const int f = frame;
		Stereo c0 = (**buffer)[f];
		Stereo c1 = ((**buffer)[f + 1] - (**buffer)[f - 1]) * 0.5;
		Stereo c2 = (**buffer)[f - 1] - (**buffer)[f] * 2.5 + (**buffer)[f + 1] * 2.0 - (**buffer)[f + 2] * 0.5;
		Stereo c3 = ((**buffer)[f + 2] - (**buffer)[f - 1]) * 0.5 + ((**buffer)[f] - (**buffer)[f + 1]) * 1.5;
		return ((c3 * x + c2) * x + c1) * x + c0;
	}

	Stereo pan (const Stereo s0, const Stereo s1) const {return panf * s1 + unpanf * s0;}

	Stereo mix (const Stereo s0, const Stereo s1, const double position, const double size, const float mixf) const
	{
		return BUtilities::mix<Stereo> (s0, pan (s0, s1), params[SLOTS_MIX] * adsr (position, size) * mixf);
	}


};

#endif /* FX_HPP_ */
