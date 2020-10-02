/* Dial.hpp
 * Copyright (C) 2018, 2019  Sven Jähnichen
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

#ifndef BWIDGETS_DIAL_HPP_
#define BWIDGETS_DIAL_HPP_

#include "RangeWidget.hpp"
#include "Knob.hpp"
#include "DrawingSurface.hpp"
#include "Label.hpp"
#include "Focusable.hpp"

#define BWIDGETS_DEFAULT_DIAL_WIDTH 50.0
#define BWIDGETS_DEFAULT_DIAL_HEIGHT 50.0
#define BWIDGETS_DEFAULT_DIAL_KNOB_SIZE 0.6
#define BWIDGETS_DEFAULT_DIAL_DEPTH 1.0
#define BWIDGETS_DEFAULT_DIAL_DOT_SIZE 0.1

namespace BWidgets
{
/**
 * Class BWidgets::Dial
 *
 * RangeWidget class dial.
 * The Widget is clickable by default.
 */
class Dial : public RangeWidget, public Focusable
{
public:
	Dial ();
	Dial (const double x, const double y, const double width, const double height, const std::string& name,
		const double value, const double min, const double max, const double step);

	/**
	 * Creates a new (orphan) dial and copies the dial properties from a
	 * source dial.
	 * @param that Source dial
	 */
	Dial (const Dial& that);

	/**
	 * Assignment. Copies the dial properties from a source dial and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible and a value changed event.
	 * @param that Source widget
	 */
	Dial& operator= (const Dial& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Changes the value of the widget and keeps it within the defined range.
	 * Passes the value to its predefined child widgets.
	 * Emits a value changed event and (if visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val) override;

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				Styles used are:
	 * 				BWIDGETS_KEYWORD_BORDER
	 * 				BWIDGETS_KEYWORD_BACKGROUND
	 * 				BWIDGETS_KEYWORD_FGCOLORS
	 * 				BWIDGETS_KEYWORD_BGCOLORS
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Handles the BEvents::BUTTON_PRESS_EVENT to turn the dial.
	 * @param event Pointer to a poiter event emitted by the same widget.
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::EventType::BUTTON_RELEASE_EVENT to turn the dial.
	 * @param event Pointer event
	 */
	virtual void onButtonReleased (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::POINTER_DRAG_EVENT to turn
	 * the dial.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::WHEEL_SCROLL_EVENT to turn
	 * the dial.
	 * @param event Pointer to a wheel event emitted by the same widget.
	 */
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::FOCUS_IN_EVENT.
	 * @param event Focus event
	 */
	virtual void onFocusIn (BEvents::FocusEvent* event) override;

	/**
	 * Predefined empty method to handle a
	 * BEvents::EventType::FOCUS_OUT_EVENT.
	 * @param event Focus event
	 */
	virtual void onFocusOut (BEvents::FocusEvent* event) override;


protected:
	void drawDot ();
	virtual void updateCoords ();
	virtual void draw (const BUtilities::RectArea& area) override;

	BUtilities::Point dialCenter;
	double dialRadius;

	Knob knob;
	DrawingSurface dot;
	Label focusLabel;
	BColors::ColorSet fgColors;
	BColors::ColorSet bgColors;
};

}


#endif /* BWIDGETS_DIAL_HPP_ */
