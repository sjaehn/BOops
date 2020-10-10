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

#ifndef BUTTERWORTHFILTER_HPP_
#define BUTTERWORTHFILTER_HPP_

#include <cmath>
#include <array>
#include "Stereo.hpp"

#define BUTTERWORTH_MAXORDER 16

class ButterworthFilter
{
public:
	ButterworthFilter() : ButterworthFilter (0) {}

	ButterworthFilter (const int order) :
		order (order),
		f1 (1)
	{
		coeff0.fill (0);
		coeff1.fill (0);
		coeff2.fill (0);
		clear();
	}

	Stereo push (const Stereo& input)
	{
		output = input;

		for (int i = 0; i < int (order / 2); ++i)
		{
			buffer0[i] = buffer1[i] * coeff1[i] + buffer2[i] * coeff2[i] + output;
			output = (buffer0[i] + buffer1[i] * f1 + buffer2[i]) * coeff0[i];
			buffer2[i] = buffer1[i];
			buffer1[i] = buffer0[i];
		}

		return output;
	}

	Stereo get() const {return output;}

	void clear()
	{
		buffer0.fill (Stereo());
		buffer1.fill (Stereo());
		buffer2.fill (Stereo());
	}


protected:
	int order;
	std::array <float, BUTTERWORTH_MAXORDER / 2> coeff0;
	std::array <float, BUTTERWORTH_MAXORDER / 2> coeff1;
	std::array <float, BUTTERWORTH_MAXORDER / 2> coeff2;
	float f1;
	std::array <Stereo, BUTTERWORTH_MAXORDER / 2> buffer0;
	std::array <Stereo, BUTTERWORTH_MAXORDER / 2> buffer1;
	std::array <Stereo, BUTTERWORTH_MAXORDER / 2> buffer2;
	Stereo output;
};

#endif /* BUTTERWORTHFILTER_HPP_ */
