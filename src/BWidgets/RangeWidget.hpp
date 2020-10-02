/* RangeWidget.hpp
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

#ifndef BWIDGETS_RANGEWIDGET_HPP_
#define BWIDGETS_RANGEWIDGET_HPP_

#define BWIDGETS_DEFAULT_RANGE_MIN 0.0
#define BWIDGETS_DEFAULT_RANGE_MAX 100.0
#define BWIDGETS_DEFAULT_RANGE_STEP 0.0

#include <math.h>
#include "ValueWidget.hpp"

namespace BWidgets
{
/**
 * Class BWidgets::RangeWidget
 *
 * Root class for all widget with a value that should kept in a range.
 */
class RangeWidget : public ValueWidget
{
public:
	RangeWidget ();
	RangeWidget (const double  x, const double y, const double width, const double height, const std::string& name,
		const double value, const double min, const double max, const double step);

	/**
	 * Creates a new (orphan) widget and copies the widget properties from a
	 * source widget. This method doesn't copy any parent or child widgets.
	 * @param that Source widget
	 */
	RangeWidget (const RangeWidget& that);

	~RangeWidget ();

	/**
	 * Assignment. Copies the widget properties from a source widget and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible and a value changed event.
	 * @param that Source widget
	 */
	RangeWidget& operator= (const RangeWidget& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Changes the value of the widget and keeps it within the defined range.
	 * Emits a value changed event and (if visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val) override;

	/**
	 * Calculates the value as fraction of the distance between the range lower
	 * and upper limit (or vice versa if negative step).
	 * @return Relative value (0..1)
	 */
	double getRelativeValue () const;

	/**
	 * Sets the lower limit. Forces the value into the new range. Emits a value
	 * changed event (if value changed) and (if visible) an expose event.
	 * @param min Lower limit
	 */
	virtual void setMin (const double min);

/**
	 * Gets the lower limit.
	 * @return Lower limit
	 */
	double getMin () const;

	/**
	 * Sets the upper limit. Forces the value into the new range. Emits a value
	 * changed event (if value changed) and (if visible) an expose event.
	 * @param max Upper limit
	 */
	virtual void setMax (const double max);

	/**
	 * Gets the upper limit.
	 * @return Upper limit
	 */
	double getMax () const;

	/**
	 * Sets the increment steps for the value.
	 * @param step Increment steps.
	 * 			   A positive value means that increments start from the lower
	 * 			   limit to the upper limit.
	 * 			   0.0 menas stepless increments.
	 * 			   A negative value however means that decrements start from
	 * 			   the upper to the lowe limit. A negative value may also cause
	 * 			   display of dependent widgets (dials, sliders, ... ) in the
	 * 			   opposite direction.
	 */
	virtual void setStep (const double step);

	/**
	 * Gets the increment steps for the value.
	 * @return Increment steps.
	 */
	double getStep () const;

	/**
	 * Sets the limits. Forces the value into the new range. Emits a value
	 * changed event (if value changed) and (if visible) an expose event.
	 * @param min Lower limit
	 */
	virtual void setLimits (const double min, const double max, const double step);

protected:
	double rangeMin;
	double rangeMax;
	double rangeStep;

};

}

#endif /* BWIDGETS_RANGEWIDGET_HPP_ */
