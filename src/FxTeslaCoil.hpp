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

#ifndef FXTESLACOIL_HPP_
#define FXTESLACOIL_HPP_

#include "Fx.hpp"

#define FX_TESLACOIL_DRIVE 0
#define FX_TESLACOIL_DRIVERAND 1
#define FX_TESLACOIL_LEVEL 2
#define FX_TESLACOIL_LEVELRAND 3

#ifndef SGN
#define SGN(x) (((x) > 0) - ((x) < 0))
#endif

#ifndef DB2CO
#define DB2CO(x) pow (10, 0.05 * (x))
#endif

class FxTestlaCoil : public Fx
{
public:
	FxTestlaCoil () = delete;

	FxTestlaCoil (RingBuffer<Stereo>** buffer, float* params, Pad* pads, const double rate) :
		Fx (buffer, params, pads),
		samplerate (rate),
		drive (0.0f),
		level (0.0f),
		lsign (0.0f),
		rsign (0.0f),
		lexc (false),
		rexc (false)
	{}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		drive = DB2CO (-30.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_TESLACOIL_DRIVE] + r1 * params[SLOTS_OPTPARAMS + FX_TESLACOIL_DRIVERAND], 0.00, 1.0));
		const double r2 = bidist (rnd);
		level = DB2CO (-70.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_TESLACOIL_LEVEL] + r2 * params[SLOTS_OPTPARAMS + FX_TESLACOIL_LEVELRAND], 0.0, 1.0));
		lsign = 0.0f;
		rsign = 0.0f;
		lexc = false;
		rexc = false;
		lt = 0.0f;
		rt = 0.0f;
		rpow = 0.0f;
		lpow = 0.0f;
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		Stereo s1 = Stereo ();
		const float ls = SGN (s0.left);
		const float rs = SGN (s0.right);

		// Polatity changed
		if (lsign != ls)
		{
			lexc = false;
			lsign = ls;
		}
	
		if (rsign != rs)
		{
			rexc = false;
			rsign = rs;
		}

		// Signnal over the limit
		if ((!lexc) && (/*fabsf*/ (s0.left * drive) > 1.0f))
		{
			lpow = level;
			lt = 0.0f;
			lexc = true;
		}

		if ((!rexc) && (/*fabsf*/ (s0.right * drive) > 1.0f))
		{
			rpow = level;
			rt = 0.0f;
			rexc = true;
		}

		// Generate sound
		if (lpow > 0)
		{
			s1.left = lpow * (sinf (2.0f * M_PI * lt) /* + 0.25f * bidist (rnd) */);
			lt += (6000.0 / samplerate) * (1 + 0.25f * bidist (rnd));
			lpow *= 0.875f;
			if (lpow < 0.0001f) lpow = 0.0f;
		}

		if (rpow > 0)
		{
			s1.right = rpow * (sinf (2.0f * M_PI * rt) /* + 0.25f * bidist (rnd) */);
			rt += (6000.0 / samplerate) * (1 + 0.25f * bidist (rnd));
			rpow *= 0.875f;
			if (rpow < 0.0001f) rpow = 0.0f;
		}

		return s1;
	}

protected:
	double samplerate;
	float drive;
	float level;
	float lsign;
	float rsign;
	bool lexc;
	bool rexc;
	float lt;
	float rt;
	float lpow;
	float rpow;


};

#endif /* FXTESLACOIL_HPP_ */
