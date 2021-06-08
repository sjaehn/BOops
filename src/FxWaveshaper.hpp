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

#ifndef FXWAVESHAPER_HPP_
#define FXWAVESHAPER_HPP_

#include "Fx.hpp"
#include "Shape.hpp"

#ifndef DB2CO
#define DB2CO(x) (pow (10, 0.05 * (x)))
#endif

#ifndef CO2DB
#define CO2DB(x) (20.0 * log10 (x))
#endif

#ifndef SGN
#define SGN(x) (((x) > 0) - ((x) < 0))
#endif

#define FX_WAVESHAPER_DRIVE 0
#define FX_WAVESHAPER_DRIVERAND 1
#define FX_WAVESHAPER_GAIN 2
#define FX_WAVESHAPER_GAINRAND 3
#define FX_WAVESHAPER_UNIT 4

class FxWaveshaper : public Fx
{
public:
	FxWaveshaper () = delete;

	FxWaveshaper (RingBuffer<Stereo>** buffer, float* params, Pad* pads, Shape<SHAPE_MAXNODES>* shape) :
		Fx (buffer, params, pads),
		shape (shape),
		drive (500.0f),
		gain (0.5f),
		unit (0)
	{
		if (!shape) throw std::invalid_argument ("Fx initialized with shape nullptr");
	}

	virtual void init (const double position) override
	{
		Fx::init (position);
		const double r1 = bidist (rnd);
		drive = DB2CO (-30.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_WAVESHAPER_DRIVE] + r1 * params[SLOTS_OPTPARAMS + FX_WAVESHAPER_DRIVERAND], 0.00, 1.0));
		const double r2 = bidist (rnd);
		gain = DB2CO (-70.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_WAVESHAPER_GAIN] + r2 * params[SLOTS_OPTPARAMS + FX_WAVESHAPER_GAINRAND], 0.0, 1.0));
		unit = LIMIT (params[SLOTS_OPTPARAMS + FX_WAVESHAPER_UNIT], 0, 1);
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		float l;
		float r;
		float lf;
		float rf;

		if (unit == 0)
		{
			l = fabsf (s0.left * drive);
			r = fabsf (s0.right * drive);
			l = LIMIT (l, 0.0f, 1.0f);
			r = LIMIT (r, 0.0f, 1.0f);
			lf = shape->getMapValue (l);
			rf = shape->getMapValue (r);
		}

		else
		{
			l = (90.0f + CO2DB (fabsf (0.000031623f + s0.left * drive))) / 120.0f;
			r = (90.0f + CO2DB (fabsf (0.000031623f + s0.right * drive))) / 120.0f;
			l = LIMIT (l, 0.0f, 1.0f);
			r = LIMIT (r, 0.0f, 1.0f);
			lf = DB2CO (-90.0f + shape->getMapValue (l) * 120.0f);
			rf = DB2CO (-90.0f + shape->getMapValue (r) * 120.0f);
		}

		const Stereo s1 = Stereo (SGN (s0.left) * lf * gain, SGN (s0.right) * rf * gain);
		return mix (s0, s1, position, size, mixf);
	}

protected:
	Shape<SHAPE_MAXNODES>* shape;
	float drive;
	float gain;
	int unit;
};

#endif /* FXWAVESHAPER_HPP_ */
