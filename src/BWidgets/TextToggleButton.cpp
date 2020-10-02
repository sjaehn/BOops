/* TextToggleButton.cpp
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

#include "TextToggleButton.hpp"

namespace BWidgets
{
TextToggleButton::TextToggleButton () : TextToggleButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT,
														  "texttogglebutton", "", 0.0) {}

TextToggleButton::TextToggleButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue) :
		TextToggleButton (x, y, width, height, name, name, defaultValue) {}

TextToggleButton::TextToggleButton (const double x, const double y, const double width, const double height,
						const std::string& name, const std::string& label, double defaultValue) :
		ToggleButton (x, y, width, height, name, defaultValue),
		buttonLabel (0, 0, width, height, name, label)
{
	buttonLabel.setClickable  (false);
	buttonLabel.setState (defaultValue ? BColors::ACTIVE : BColors::NORMAL);
	buttonLabel.setBorder (BWIDGETS_DEFAULT_MENU_TEXTBORDER);
	add (buttonLabel);
}

TextToggleButton::TextToggleButton (const TextToggleButton& that) : ToggleButton (that), buttonLabel (that.buttonLabel)
{
	add (buttonLabel);
}

TextToggleButton:: ~TextToggleButton () {}

TextToggleButton& TextToggleButton::operator= (const TextToggleButton& that)
{
	release (&buttonLabel);

	ToggleButton::operator= (that);
	buttonLabel = that.buttonLabel;

	add (buttonLabel);

	return *this;
}

Widget* TextToggleButton::clone () const {return new TextToggleButton (*this);}

void TextToggleButton::setWidth (const double width)
{
	Button::setWidth (width);
	buttonLabel.setWidth (width);
}

void TextToggleButton::setHeight (const double height)
{
	Button::setHeight (height);
	buttonLabel.setHeight (height);
}

void TextToggleButton::resize ()
{
	buttonLabel.resize ();
	Widget::resize ();
}

void TextToggleButton::resize (const double width, const double height) {TextToggleButton::resize (BUtilities::Point (width, height));}

void TextToggleButton::resize (const BUtilities::Point extends)
{
	Widget::resize (BUtilities::Point (extends.x, extends.y));
	buttonLabel.resize (BUtilities::Point (extends.x, extends.y));
}

void TextToggleButton::setValue (const double val)
{
	if (val) buttonLabel.setState (BColors::ACTIVE);
	else buttonLabel.setState (BColors::NORMAL);
	Button::setValue (val);
}

Label* TextToggleButton::getLabel () {return &buttonLabel;}

void TextToggleButton::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}
void TextToggleButton::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	ToggleButton::applyTheme (theme, name);
	buttonLabel.applyTheme (theme, name);
	update ();
}

}
