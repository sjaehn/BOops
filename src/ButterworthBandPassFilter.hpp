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

#ifndef BUTTERWORTHBANDPASSFILTER_HPP_
#define BUTTERWORTHBANDPASSFILTER_HPP_

#include "ButterworthLowPassFilter.hpp"
#include "ButterworthHighPassFilter.hpp"

class ButterworthBandPassFilter
{
public:
	ButterworthBandPassFilter (const double rate, const double lowCutoff, const double highCutoff, const int order) :
		lowpass (rate, highCutoff, order),
		highpass (rate, lowCutoff, order)
	{

	}

	void set (const double rate, const double lowCutoff, const double highCutoff, const int order)
	{
		lowpass.set (rate, highCutoff, order);
		highpass.set (rate, lowCutoff, order);
	}

	Stereo push (const Stereo& input) {return highpass.push (lowpass.push (input));}

	Stereo get () const {return highpass.get();}

	void clear()
	{
		lowpass.clear();
		highpass.clear();
	}


protected:
	ButterworthLowPassFilter lowpass;
	ButterworthHighPassFilter highpass;
};

#endif /* BUTTERWORTHBANDPASSFILTER_HPP_ */
