/* ValueWidget.cpp
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

#include "ValueWidget.hpp"

namespace BWidgets
{
ValueWidget::ValueWidget () : ValueWidget (0, 0, BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, "valuewidget", BWIDGETS_DEFAULT_VALUE) {}

ValueWidget::ValueWidget (const double  x, const double y, const double width, const double height, const std::string& name, const double value) :
		Widget (x, y, width, height, name), value (value), valueable_ (true), hardChangeable (true), softValue (0.0) {}

ValueWidget::ValueWidget (const ValueWidget& that) :
		Widget (that), value (that.value), valueable_ (that.valueable_), hardChangeable (that.hardChangeable), softValue (that.softValue) {}

ValueWidget::~ValueWidget () {}

ValueWidget& ValueWidget::operator= (const ValueWidget& that)
{
	Widget::operator= (that);
	valueable_ = that.valueable_;
	hardChangeable = that.hardChangeable;
	softValue = that.softValue;
	setValue (that.value);
	return *this;
}

Widget* ValueWidget::clone () const {return new ValueWidget (*this);}

void ValueWidget::setValue (const double val)
{
	if (val != value)
	{
		value = val;
		softValue = 0.0;
		update ();
		if (valueable_) postValueChanged ();
	}
}

double ValueWidget::getValue () const {return value;}

void ValueWidget::setValueable (const bool status) {valueable_ = status;}

bool ValueWidget::isValueable () const {return valueable_;}

void ValueWidget::setHardChangeable (const bool status) {hardChangeable = status;}

bool ValueWidget::isHardChangeable () const {return hardChangeable;}

void ValueWidget::postValueChanged ()
{
	if (main_)
	{
		BEvents::ValueChangedEvent* event = new BEvents::ValueChangedEvent (this, value);
		main_->addEventToQueue (event);
	}
}

}
