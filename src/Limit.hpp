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

#ifndef LIMIT_HPP_
#define LIMIT_HPP_

#include <cmath>

struct Limit
{
	float min;
	float max;
	float step;

        float validate (float value) const
        {
                if (max <= min) return min;
                if (value <= min) return min;
                if (value >= max) return max;
                if (step == 0) return value;
		float newValue = (step > 0 ? min + round ((value - min) / step) * step : max - round ((max - value) / step) * step);
		return (newValue >= min ? (newValue <= max ? newValue : max) : min);
        }
};

#endif /* LIMIT_HPP_ */
