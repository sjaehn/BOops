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

#ifndef FXBANGER_HPP_
#define FXBANGER_HPP_

#include "Fx.hpp"
#include "Airwindows/XRegion.hpp"

#define FX_BANGER_SPEED 8
#define FX_BANGER_SPEEDRAND 9
#define FX_BANGER_SPIN 10
#define FX_BANGER_SPINRAND 11

class FxBanger : public Fx
{
public:
	FxBanger () = delete;

	FxBanger (RingBuffer<Stereo>** buffer, float* params, Pad* pads, double rate) :
		Fx (buffer, params, pads),
		rate (rate),
		count (0.0),
		xregion (rate),
		xparams {{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
		speed (0.0f),
		spin (0.0f),
		ang (0.0f),
		nspeed (0.0f),
		nspin (0.0f),
		xcursor (0.0f),
		ycursor (0.0f)

	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		for (int i = 0; i < 4; ++i)
		{
			
			for (int j = 0; j < 4; ++j)
			{
				const double r = bidist (rnd);
				xparams[i][j] = LIMIT (params[SLOTS_OPTPARAMS + 2 * j] + r * params[SLOTS_OPTPARAMS + 2 * j + 1], 0.0, 1.0);
			}
		}
	}

	virtual Stereo process (const double position, const double size) override
	{
		if (count >= rate)
		{
			nspeed = LIMIT (params[SLOTS_OPTPARAMS + FX_BANGER_SPEED] + bidist (rnd) * params[SLOTS_OPTPARAMS + FX_BANGER_SPEEDRAND], 0.0f, 1.0f);
			nspin = 2.0f * LIMIT (params[SLOTS_OPTPARAMS + FX_BANGER_SPIN] + bidist (rnd) * params[SLOTS_OPTPARAMS + FX_BANGER_SPINRAND], 0.0f, 1.0f) - 1.0f;
			count = 0.0;
		}

		else count++;

		// Update speed
		speed = (1.0 - 1.0 / rate) * speed + (1.0 / rate) * nspeed;

		// Update ang
		spin = (1.0 - 1.0 / rate) * spin + (1.0 / rate) * nspin;
		ang += 2.0 * M_PI * (10.0 / rate) * spin;

		// Calulate new positions
		const float dx = sinf (ang);
		const float dy = cosf (ang);
		xcursor += dx * (1.0 / rate) * 4.0 * speed * speed;
		ycursor += dy * (1.0 / rate) * 4.0 * speed * speed;

		// Reflections
		if (xcursor < 0.0f)
		{
			xcursor = 0.0f;
			if (dx < 0.0f) {ang = -ang; spin = 0.0f;}
		}

		else if (xcursor > 1.0f)
		{
			xcursor = 1.0f;
			if (dx > 0.0f) {ang = -ang; spin = 0.0f;}
		}

		if (ycursor < 0.0f)
		{
			ycursor = 0.0f;
			if (dy < 0.0f) {ang = -M_PI - ang; spin = 0.0f;}
		}

		else if (ycursor > 1.0f)
		{
			ycursor = 1.0f;
			if (dy > 0.0f) {ang = -M_PI - ang; spin = 0.0f;}
		}

		// Calculate params for cursor position
		float xp[6] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
		for (int i = 0; i < 4; ++i)
		{
			xp[i] =
			(
				((1.0f - xcursor) * (1.0f - ycursor) * xparams[0][i]) +
				((1.0f - xcursor) * ycursor * xparams[1][i]) +
				(xcursor * ycursor * xparams[2][i]) +
				(xcursor * (1.0 - ycursor) * xparams[3][i])
			);
		}

		xregion.setParameters (xp);
		Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo();
		xregion.process (&s0.left, &s0.right, &s1.left, &s1.right, 1);
		return s1;
	}

protected:
	double rate;
	double count;
	XRegion xregion;
	float xparams[4][4];
	float speed;
	float spin;
	float ang;
	float nspeed;
	float nspin;
	float xcursor;
	float ycursor;
};

#endif /* FXBANGER_HPP_ */
