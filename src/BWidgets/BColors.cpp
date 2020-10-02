/* BColors.cpp
 * Copyright (C) 2018  Sven Jähnichen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "BColors.hpp"

#ifndef LIMIT
#define LIMIT(val, min, max) (val < min ? min : (val < max ? val : max))
#endif

namespace BColors
{

/*****************************************************************************
 * Class BColors::Color
 *****************************************************************************/

Color::Color () : Color (0.0, 0.0, 0.0, 0.0) {};

Color::Color (const double red, const double green, const double blue, const double alpha) :
		red_ (LIMIT (red, 0.0, 1.0)), green_ (LIMIT (green, 0.0, 1.0)), blue_ (LIMIT (blue, 0.0, 1.0)), alpha_ (LIMIT (alpha, 0.0, 1.0)) {}

Color::Color (const uint32_t red32, const uint32_t green32, const uint32_t blue32, const uint32_t alpha32) :
		red_ (red32 / 0xFFFF), green_ (green32 / 0xFFFF), blue_ (blue32 / 0xFFFF), alpha_ (alpha32 / 0xFFFF) {}

bool Color::operator== (const Color& that) const {return (0 == compare (that));}

bool Color::operator!= (const Color& that) const {return !(0 == compare (that));}

void Color::setRGBA (const double red, const double green, const double blue, const double alpha)
{

	red_ = LIMIT (red, 0.0, 1.0);
	green_ = LIMIT (green, 0.0, 1.0);
	blue_ = LIMIT (blue, 0.0, 1.0);
	alpha_ = LIMIT (alpha, 0.0, 1.0);
}

void Color::setRGB (const double red, const double green, const double blue)
{
	red_ = LIMIT (red, 0.0, 1.0);
	green_ = LIMIT (green, 0.0, 1.0);
	blue_ = LIMIT (blue, 0.0, 1.0);
}

void Color::setAlpha (const double alpha)
{
	alpha_ = LIMIT (alpha, 0.0, 1.0);
}

double Color::getRed() const {return red_;}
double Color::getGreen() const {return green_;}
double Color::getBlue() const {return blue_;}
double Color::getAlpha() const {return alpha_;}

int Color::compare (const Color& that) const
{
	return ((red_ == that.red_) && (green_ == that.green_) && (blue_ == that.blue_) && (alpha_ == that.alpha_) ? 0 : 1);
}

void Color::applyBrightness (const double brightness)
{
	double b = LIMIT (brightness, -1.0, 1.0);
	if (b < 0)
	{
		red_ = red_ * (b + 1.0);
		green_ = green_ * (b + 1.0);
		blue_ = blue_ * (b + 1.0);
	}
	else if (b > 0)
	{
		red_ = red_ + (1.0 - red_) * b;
		green_ = green_ + (1.0 - green_) * b;
		blue_ = blue_ + (1.0 - blue_) * b;
	}
}

/*
 * End of class BColors::Color
 *****************************************************************************/

/*****************************************************************************
 * Class BColors::ColorSet
 *****************************************************************************/

ColorSet::ColorSet () : ColorSet ({grey, lightgrey, darkgrey}) {};
ColorSet::ColorSet (const std::vector<Color> vectorOfColors) : colors (vectorOfColors) {};

bool ColorSet::operator== (const ColorSet& that) const {return (colors == that.colors);}

bool ColorSet::operator!= (const ColorSet& that) const {return (colors != that.colors);}

int ColorSet::compare (const ColorSet& that) const {return (colors == that.colors ? 0 : 1);}

void ColorSet::addColor (const State state, const Color& color)
{
	// Filling undefined vector elements with Color invisible
	int size = colors.size ();
	for (int i = size; i <= (int) state; ++i) colors.push_back (invisible);

	colors[state] = color;
}

void ColorSet::removeColor (const State state)
{
	if (state < colors.size ())
	{
		colors[state] = invisible;
	}

	// TODO shrink vector colors if last element is removed
}

Color* ColorSet::getColor (const State state)
{
	if (state < colors.size ())
	{
		return &colors[state];
	}
	else
	{
		return &noColor;
	}
}

/*
 * End of class BColors::ColorSet
 *****************************************************************************/

}
