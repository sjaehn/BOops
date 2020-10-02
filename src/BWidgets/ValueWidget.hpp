/* ValueWidget.hpp
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

#ifndef BWIDGETS_VALUEWIDGET_HPP_
#define BWIDGETS_VALUEWIDGET_HPP_

#include "Widget.hpp"
#include "Window.hpp"

#define BWIDGETS_DEFAULT_VALUE 0.0
#define BWIDGETS_DEFAULT_VALUE_FORMAT "%3.2f"

namespace BWidgets
{

/**
 * Class BWidgets::ValueWidget
 *
 * Root class for all widgets that deal with values (dials, sliders, ...).
 */
class ValueWidget : public Widget
{
public:
	ValueWidget ();
	ValueWidget (const double x, const double y, const double width, const double height, const std::string& name, const double value);

	/**
	 * Creates a new (orphan) widget and copies the widget properties from a
	 * source widget. This method doesn't copy any parent or child widgets.
	 * @param that Source widget
	 */
	ValueWidget (const ValueWidget& that);

	~ValueWidget ();

	/**
	 * Assignment. Copies the widget properties from a source widget and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible and a value changed event.
	 * @param that Source widget
	 */
	ValueWidget& operator= (const ValueWidget& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Changes the value of the widget. Emits a value changed event and (if
	 * visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val);

	/**
	 * Gets the value of the widget.
	 * @return Value
	 */
	virtual double getValue () const;

	virtual void setValueable (const bool status);

	virtual bool isValueable () const;

	/**
	 * Defines whether the widget may allow direct setting of a value by
	 * clicking or it only allows relative changes by dragging.
	 * @param status TRUE if direct setting is allowed, otherwise false
	 */
	void setHardChangeable (const bool status);

	/**
	 * Gets whether the widget may allow direct setting of a value by
	 * clicking or it only allows relative changes by dragging.
	 * @return TRUE if direct setting is allowed, otherwise false
	 */
	bool isHardChangeable () const;

protected:
	void postValueChanged ();
	double value;
	bool valueable_;
	bool hardChangeable;
	double softValue;
};

}

#endif /* BWIDGETS_VALUEWIDGET_HPP_ */
