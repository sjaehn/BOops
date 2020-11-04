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

#ifndef CRACKLE_HPP_
#define CRACKLE_HPP_

#include <cmath>

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

class Crackle
{
public:
	Crackle () : Crackle (0, 0, 0) {}

	Crackle (const double t0, const double level, const double freq) :
		t0_ (t0),
		level_ (level),
		freq_ (freq)
	{}

	double get (const double t) const {return level_ * sin (2.0 * M_PI * sqrt ((t - t0_) * freq_)) / (1.0 + SQR ((t - t0_) * freq_ - 2.0));}

	double getAmplitude (const double t) const {return 1.0 / (1.0 + SQR ((t - t0_) * freq_ - 2.0));}

protected:
	double t0_;
	double level_;
	double freq_;
};

#endif /* CRACKLE_HPP_ */
