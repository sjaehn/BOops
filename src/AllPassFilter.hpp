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

#ifndef ALLPASSFILTER_HPP_
#define ALLPASSFILTER_HPP_

class AllPassFilter
{
public:
	AllPassFilter() : AllPassFilter (0) {}

	AllPassFilter (const float delay) : f (0), m (0) {setDelay (delay);}

	void setDelay (const float delay) {f = (1 - delay) / (1 + delay);}

	float process (const float input)
	{
		const float r = m  - input * f;
		m = input + r * f;
		return r;
	}

protected:
	float f;
	float m;
};

#endif /* ALLPASSFILTER_HPP_ */
