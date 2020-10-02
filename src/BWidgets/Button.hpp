/* Button.hpp
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

#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include "ValueWidget.hpp"

#define BWIDGETS_DEFAULT_BUTTON_WIDTH 50.0
#define BWIDGETS_DEFAULT_BUTTON_HEIGHT 20.0
#define BWIDGETS_KEYWORD_BUTTONCOLORS "buttoncolors"
#define BWIDGETS_DEFAULT_BUTTON_BORDER 1.0
#define BWIDGETS_DEFAULT_BUTTON_RAD 4.0

namespace BWidgets
{
/**
 * Class BWidgets::Button
 *
 * Basic button widget drawing a button. Is is a BWidgets::ValueWidget having
 * two conditions: on (value != 0) or off (value == 0). The Widget is clickable
 * by default.
 */
class Button : public ValueWidget
{
public:
	Button ();
	Button (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue = 0.0);

	/**
	 * Creates a new (orphan) button and copies the button properties from a
	 * source button. This method doesn't copy any parent or child widgets.
	 * @param that Source button
	 */
	Button (const Button& that);

	~Button ();

	/**
	 * Assignment. Copies the widget properties from a source button and keeps
	 * its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param that Source button
	 */
	Button& operator= (const Button& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				Styles used are:
	 * 				"buttoncolors" for BStyles::ColorSet
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Handles the BEvents::BUTTON_PRESS_EVENT.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::BUTTON_RELEASED_EVENT.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onButtonReleased (BEvents::PointerEvent* event) override;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	BColors::ColorSet bgColors;
};

}

#endif /* BUTTON_HPP_ */
