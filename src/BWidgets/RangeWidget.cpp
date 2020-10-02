/* RangeWidget.cpp
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

#include "RangeWidget.hpp"

#ifndef LIMIT
#define LIMIT(val, min, max) (val < min ? min : (val < max ? val : max))
#endif

namespace BWidgets
{
RangeWidget::RangeWidget () :
	RangeWidget (0.0, 0.0, BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, "rangewidget",
		BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP) {}

RangeWidget::RangeWidget (const double  x, const double y, const double width, const double height, const std::string& name,
	const double value, const double min, const double max, const double step) :
	ValueWidget (x, y, width, height, name, value), rangeMin (min <= max ? min : max),
	rangeMax (max), rangeStep (step)
{
	this->value = LIMIT (value, min, max);
}

RangeWidget::RangeWidget (const RangeWidget& that) :
	ValueWidget (that), rangeMin (that.rangeMin <= that.rangeMax ? that.rangeMin : that.rangeMax), rangeMax (that.rangeMax), rangeStep (that.rangeStep) {}

RangeWidget::~RangeWidget () {}

RangeWidget& RangeWidget::operator= (const RangeWidget& that)
{
	Widget::operator= (that);
	rangeMin = that.rangeMin;
	rangeMax = that.rangeMax;
	rangeStep = that.rangeStep;
	setValue (that.value);

	return *this;
}

Widget* RangeWidget::clone () const {return new RangeWidget (*this);}

void RangeWidget::setValue (const double val)
{
	double valRounded = LIMIT (val, rangeMin, rangeMax);
	if ((rangeStep != 0.0) && (rangeMax >= rangeMin))
	{
		if (rangeStep > 0.0) valRounded = LIMIT (rangeMin + round ((val - rangeMin) / rangeStep) * rangeStep, rangeMin, rangeMax);
		else valRounded = LIMIT (rangeMax - round ((rangeMax - val) / rangeStep) * rangeStep, rangeMin, rangeMax);
	}

	if (value != valRounded) ValueWidget::setValue (valRounded);
}

double RangeWidget::getRelativeValue () const
{
	double relVal;
	if (getMax () != getMin ()) relVal = (getValue () - getMin ()) / (getMax () - getMin ());
	else relVal = 0.5;				// min == max doesn't make any sense, but need to be handled
	if (getStep() < 0) relVal = 1 - relVal;		// Swap if reverse orientation
	return relVal;
}

void RangeWidget::setMin (const double min)
{
	double newMin = (min <= rangeMax ? min: rangeMax);
	if (rangeMin != newMin)
	{
		rangeMin = newMin;
		if (getValue () < rangeMin) setValue (rangeMin);
		update ();
	}
}

double RangeWidget::getMin () const {return rangeMin;}

void RangeWidget::setMax (const double max)
{
	double newMax = (max >= rangeMin ? max: rangeMin);
	if (rangeMax != newMax)
	{
		rangeMax = newMax;
		if (getValue () > rangeMax) setValue (rangeMax);
		update ();
	}
}

double RangeWidget::getMax () const {return rangeMax;}

void RangeWidget::setStep (const double step) {rangeStep = step;}

double RangeWidget::getStep () const {return rangeStep;}

void RangeWidget::setLimits (const double min, const double max, const double step)
{
	double newMin = min <= max ? min : max;

	if ((newMin != rangeMin) || (max != rangeMax) || (step != rangeStep))
	{
		rangeMin = newMin;
		rangeMax = max;
		rangeStep = step;
		if (getValue () < rangeMin) setValue (rangeMin);
		else if (getValue () > rangeMax) setValue (rangeMax);
		update ();
	}
}


}
