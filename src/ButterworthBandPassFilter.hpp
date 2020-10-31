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

#ifndef BUTTERWORTHBANDPASSFILTER_HPP_
#define BUTTERWORTHBANDPASSFILTER_HPP_

#include "ButterworthFilter.hpp"


class ButterworthBandPassFilter : public ButterworthFilter
{
public:
	ButterworthBandPassFilter(const double rate, const double lowCutoff, const double highCutoff, const int order) :
		ButterworthFilter (order)
	{
		coeff3.fill (0);
		coeff4.fill (0);
		clear();

		const double a = cos (M_PI * (highCutoff + lowCutoff) / rate) / cos (M_PI * (highCutoff - lowCutoff) / rate);
		const double a2 = a * a;
		const double b = tan (M_PI * (highCutoff - lowCutoff) / rate);
		const double b2 = b * b;

		for (int i = 0; i < int (order / 2); ++i)
		{
			const double r = sin (M_PI * (2.0 * double (i) + 1.0) / (2.0 * double (order)));
			const double s = b2 + 2.0 * b * r + 1.0;
			coeff0[i] = b2 / s;
			coeff1[i] = 4.0 * a * (1.0 + b * r) / s;
			coeff2[i] = 2.0 * (b2 - 2.0 * a2 - 1.0) / s;
			coeff3[i] = 4.0 * a * (1.0 - b * r) / s;
			coeff4[i] = -(b2 - 2.0 * b * r + 1.0) / s;
		}

		f1 = -2;
	}

	Stereo push (const Stereo& input)
	{
		output = input;

		for (int i = 0; i < int (order / 2); ++i)
		{
			buffer0[i] = buffer1[i] * coeff1[i] + buffer2[i] * coeff2[i] + buffer3[i] * coeff3[i] + buffer4[i] * coeff4[i] + output;
			output = (buffer0[i] + buffer2[i] * f1 + buffer4[i]) * coeff0[i];
			buffer4[i] = buffer3[i];
			buffer3[i] = buffer2[i];
			buffer2[i] = buffer1[i];
			buffer1[i] = buffer0[i];
		}

		return output;
	}

	void clear()
	{
		ButterworthFilter::clear();
		buffer3.fill (Stereo());
		buffer4.fill (Stereo());
	}


protected:
	std::array <float, BUTTERWORTH_MAXORDER / 2> coeff3;
	std::array <float, BUTTERWORTH_MAXORDER / 2> coeff4;
	std::array <Stereo, BUTTERWORTH_MAXORDER / 2> buffer3;
	std::array <Stereo, BUTTERWORTH_MAXORDER / 2> buffer4;
};

#endif /* BUTTERWORTHBANDPASSFILTER_HPP_ */
