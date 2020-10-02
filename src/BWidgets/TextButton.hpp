/* TextButton.hpp
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

#ifndef BWIDGETS_TEXTBUTTON_HPP_
#define BWIDGETS_TEXTBUTTON_HPP_

#include "Button.hpp"
#include "Label.hpp"

namespace BWidgets
{
/**
 * Class BWidgets::TextButton
 *
 * Text button widget. Is is a BWidgets::Button and thus a
 * BWidgets::ValueWidget having two conditions: on (value != 0) or off
 * (value == 0)
 */
class TextButton : public Button
{
public:
	TextButton ();
	TextButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue = 0.0);
	TextButton (const double x, const double y, const double width, const double height,
				const std::string& name, const std::string& label, double defaultValue = 0.0);

	/**
	 * Creates a new (orphan) button and copies the button properties from a
	 * source button.
	 * @param that Source button
	 */
	TextButton (const TextButton& that);

	~TextButton ();

	/**
	 * Assignment. Copies the widget properties from a source button and keeps
	 * its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param that Source button
	 */
	TextButton& operator= (const TextButton& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Resizes the widget and its predefined child, redraw and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param width New widgets width
	 */
	virtual void setWidth (const double width) override;

	/**
	 * Resizes the widget and its predefined child, redraw and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param height New widgets height
	 */
	virtual void setHeight (const double height) override;

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible. If no parameters are given, the widget will be
	 * resized to the size of the containing child widgets.
	 * @param width		New widgets width
	 * @param height	New widgets height
	 * @param extends	New widget extends
	 */
	virtual void resize () override;
	virtual void resize (const double width, const double height) override;
	virtual void resize (const BUtilities::Point extends) override;

	/**
	 * Changes the value of the widget (0.0 == off,  0.0 != off) and relocates
	 * the embedded label widget..
	 * Emits a value changed event and (if visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val) override;

	/**
	 * Gets (a pointer to) the Label for direct access.
	 * @return Pointer to the label
	 */
	Label* getLabel ();

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				For styles used see BWidgets::Button::applyTheme and
	 * 				BWidgets::Label::applyTheme.
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

protected:
	Label buttonLabel;
};

}

#endif /* BWIDGETS_TOGGLEBUTTON_HPP_ */
