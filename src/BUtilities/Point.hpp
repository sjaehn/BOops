/* Point.hpp
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
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

#ifndef BUTILITIES_POINT_HPP_
#define BUTILITIES_POINT_HPP_

namespace BUtilities
{

struct Point
{
	double x, y;

	Point () : Point (0, 0) {}
	Point (double x, double y) : x (x), y (y) {}

	Point& operator+= (const Point& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	Point& operator-= (const Point& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	friend bool operator== (const Point& lhs, const Point& rhs) {return ((lhs.x == rhs.x) && (lhs.y == rhs.y));}
	friend bool operator!= (const Point& lhs, const Point& rhs) {return !(lhs == rhs);}
	friend Point operator+ (Point lhs, const Point& rhs) {return (lhs += rhs);}
	friend Point operator- (Point lhs, const Point& rhs) {return (lhs -= rhs);}

};

}

#endif /* BUTILITIES_POINT_HPP_ */
