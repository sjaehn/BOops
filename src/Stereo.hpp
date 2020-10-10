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

#ifndef STEREO_HPP_
#define STEREO_HPP_

#include "BUtilities/mix.hpp"

struct Stereo
{
	float left;
	float right;

	Stereo() : Stereo (0, 0) {}

	Stereo (const float left, const float right) : left (left), right (right) {}

	Stereo& operator+= (const Stereo& rhs)
	{
		left += rhs.left;
		right += rhs.right;
		return *this;
	}

	Stereo& operator-= (const Stereo& rhs)
	{
		left -= rhs.left;
		right -= rhs.right;
		return *this;
	}

	Stereo& operator*= (const Stereo& rhs)
	{
		left *= rhs.left;
		right *= rhs.right;
		return *this;
	}

	Stereo& operator/= (const Stereo& rhs)
	{
		left /= rhs.left;
		right /= rhs.right;
		return *this;
	}

	Stereo& operator+= (const float rhs)
	{
		left += rhs;
		right += rhs;
		return *this;
	}

	Stereo& operator-= (const float rhs)
	{
		left -= rhs;
		right -= rhs;
		return *this;
	}

	Stereo& operator*= (const float rhs)
	{
		left *= rhs;
		right *= rhs;
		return *this;
	}

	Stereo& operator/= (const float rhs)
	{
		left /= rhs;
		right /= rhs;
		return *this;
	}

	friend bool operator== (const Stereo& lhs, const Stereo& rhs) {return ((lhs.left == rhs.left) && (lhs.right == rhs.right));}
	friend bool operator!= (const Stereo& lhs, const Stereo& rhs) {return !(lhs == rhs);}
	friend Stereo operator+ (Stereo lhs, const Stereo& rhs) {return (lhs += rhs);}
	friend Stereo operator- (Stereo lhs, const Stereo& rhs) {return (lhs -= rhs);}
	friend Stereo operator* (Stereo lhs, const Stereo& rhs) {return (lhs *= rhs);}
	friend Stereo operator/ (Stereo lhs, const Stereo& rhs) {return (lhs /= rhs);}
	friend Stereo operator+ (Stereo lhs, const float rhs) {return (lhs += rhs);}
	friend Stereo operator- (Stereo lhs, const float rhs) {return (lhs -= rhs);}
	friend Stereo operator* (Stereo lhs, const float rhs) {return (lhs *= rhs);}
	friend Stereo operator/ (Stereo lhs, const float rhs) {return (lhs /= rhs);}

	Stereo pan (const float f) const
	{
		return
		{
			(f <= 0.0f ? left : (1.0f - f) * left),
			(f < 0.0f ? (f + 1.0f) * right : right)
		};
	}

	Stereo mix (const Stereo& s1, const float f)
	{
		*this =  BUtilities::mix (*this, s1, f);
		return *this;
	}

	Stereo mix (const Stereo& s1, const Stereo& f)
	{
		left =  BUtilities::mix (left, s1.left, f.left);
		right = BUtilities::mix (right, s1.right, f.right);
		return *this;
	}
};

#endif /* PAD_HPP_ */
