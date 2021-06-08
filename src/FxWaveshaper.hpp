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

#ifndef SGN
#define SGN(x) (((x) > 0) - ((x) < 0))
#endif

#define FX_WAVESHAPER_DRIVE 0
#define FX_WAVESHAPER_DRIVERAND 1
#define FX_WAVESHAPER_GAIN 2
#define FX_WAVESHAPER_GAINRAND 3

class FxWaveshaper : public Fx
{
public:
	FxWaveshaper () = delete;

	FxWaveshaper (RingBuffer<Stereo>** buffer, float* params, Pad* pads, Shape<SHAPE_MAXNODES>* shape) :
		Fx (buffer, params, pads),
		shape (shape),
		drive (500.0f),
		gain (0.5f)
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
	}

	virtual Stereo play (const double position, const double size, const double mixf) override
	{
		const Stereo s0 = (**buffer).front();
		if (!playing) return s0;

		float l = fabsf (s0.left * drive);
		float r = fabsf (s0.right * drive);
		const float lf = (l <= 1.0f ? shape->getMapValue (l) : shape->getMapValue (1.0) * (100.0f * l - 99.0) + 100.0f * shape->getMapValue (0.99) * (1.0f - l));
		const float rf = (r <= 1.0f ? shape->getMapValue (r) : shape->getMapValue (1.0) * (100.0f * r - 99.0) + 100.0f * shape->getMapValue (0.99) * (1.0f - r));
		const Stereo s1 = Stereo (SGN (s0.left) * lf * gain, SGN (s0.right) * rf * gain);

		return mix (s0, s1, position, size, mixf);
	}

protected:
	Shape<SHAPE_MAXNODES>* shape;
	float drive;
	float gain;
};

#endif /* FXWAVESHAPER_HPP_ */
