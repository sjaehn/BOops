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

#ifndef FXDISTORTION_HPP_
#define FXDISTORTION_HPP_

#include "Fx.hpp"

#define FX_DISTORTION_METHOD 0
#define FX_DISTORTION_DRIVE 1
#define FX_DISTORTION_DRIVERAND 2
#define FX_DISTORTION_LEVEL 3
#define FX_DISTORTION_LEVELRAND 4

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

#ifndef SGN
#define SGN(x) ((0 < (x)) - ((x) < 0))
#endif

#ifndef DB2CO
#define DB2CO(x) pow (10, 0.05 * (x))
#endif

class FxDistortion : public Fx
{
public:
	FxDistortion () = delete;

	FxDistortion (RingBuffer<Stereo>** buffer, float* params, Pad* pads) :
		Fx (buffer, params, pads),
		method (OVERDRIVE), drive (1.0), level (1.0) {}

	virtual void init (const double position) override
	{
		Fx::init (position);
		method = BOopsDistortionIndex (LIMIT (int (round (params[SLOTS_OPTPARAMS + FX_DISTORTION_METHOD] * 8)), 0, 4));
		const double r1 = bidist (rnd);
		drive = DB2CO (-30.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_DISTORTION_DRIVE] + r1 * params[SLOTS_OPTPARAMS + FX_DISTORTION_DRIVERAND], 0.00, 1.0));
		const double r2 = bidist (rnd);
		level = DB2CO (-70.0 + 100.0 * LIMIT (params[SLOTS_OPTPARAMS + FX_DISTORTION_LEVEL] + r2 * params[SLOTS_OPTPARAMS + FX_DISTORTION_LEVELRAND], 0.0, 1.0));
	}

	virtual Stereo process (const double position, const double size) override
	{
		const Stereo s0 = (**buffer).front();
		double l = s0.left * drive / level;
		double r = s0.right * drive / level;
		switch (method)
		{
			case HARDCLIP:
				l = LIMIT (l * level, -level, level);
				r = LIMIT (r * level, -level, level);
				break;

			case SOFTCLIP:
				l = SGN (l) * level * sqrt (SQR (l) / (1.0 + SQR (l)));
				r = SGN (r) * level * sqrt (SQR (r) / (1.0 + SQR (r)));
				break;

			case FOLDBACK:
				l = fabs (l) <= 1.0 ? level * l : (SGN (l) * level * float (2 * (int ((abs (l) + 1) / 2) % 2) - 1) * (1.0 - fmod (fabs (l) + 1.0, 2.0)));
				r = fabs (r) <= 1.0 ? level * r : (SGN (r) * level * float (2 * (int ((abs (r) + 1) / 2) % 2) - 1) * (1.0 - fmodf(fabs (r) + 1.0, 2.0)));
				break;

			case OVERDRIVE:
				l =
				(
					fabs (l) < (1.0/3.0) ?
					2.0 * level * l :
					(
						fabs (l) < (2.0/3.0) ?
						SGN (l) * level * (3.0 - SQR (2.0 - 3.0 * fabs (l))) / 3.0 :
						level * SGN (l)
					)
				);

				r =
				(
					fabs (r) < (1.0/3.0) ?
					2.0 * level * r :
					(
						fabs (r) < (2.0/3.0) ?
						SGN (r) * level * (3.0 - SQR (2.0 - 3.0 * fabs (r))) / 3.0 :
						level * SGN (r)
					)
				);
				break;

			case FUZZ:
				l = SGN (l) * level * (1.0 - expf (- fabs (l)));
				r = SGN (r) * level * (1.0 - expf (- fabs (r)));
				break;

			default:
				break;
		}

		return Stereo (l, r);
	}

protected:
	BOopsDistortionIndex method;
	double drive;
	double level;
};

#endif /* FXDISTORTION_HPP_ */
