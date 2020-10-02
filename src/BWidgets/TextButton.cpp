/* TextButton.cpp
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

#include "TextButton.hpp"

namespace BWidgets
{
TextButton::TextButton () :
		TextButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "textbutton", "", 0.0) {}

TextButton::TextButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue) :
		TextButton (x, y, width, height, name, name, defaultValue) {}

TextButton::TextButton (const double x, const double y, const double width, const double height,
						const std::string& name, const std::string& label, double defaultValue) :
		Button (x, y, width, height, name, defaultValue),
		buttonLabel (0, 0, width, height, name, label)
{
	buttonLabel.setClickable  (false);
	buttonLabel.setState (defaultValue ? BColors::ACTIVE : BColors::NORMAL);
	buttonLabel.setBorder (BWIDGETS_DEFAULT_MENU_TEXTBORDER);
	add (buttonLabel);
}

TextButton::TextButton (const TextButton& that) : Button (that), buttonLabel (that.buttonLabel)
{
	add (buttonLabel);
}

TextButton:: ~TextButton () {}

TextButton& TextButton::operator= (const TextButton& that)
{
	release (&buttonLabel);

	Button::operator= (that);
	buttonLabel = that.buttonLabel;

	add (buttonLabel);

	return *this;
}

Widget* TextButton::clone () const {return new TextButton (*this);}

void TextButton::setWidth (const double width)
{
	Button::setWidth (width);
	buttonLabel.setWidth (width);
}

void TextButton::setHeight (const double height)
{
	Button::setHeight (height);
	buttonLabel.setHeight (height);
}

void TextButton::resize ()
{
	buttonLabel.resize ();
	Widget::resize ();
}

void TextButton::resize (const double width, const double height) {TextButton::resize (BUtilities::Point (width, height));}

void TextButton::resize (const BUtilities::Point extends)
{
	Widget::resize (BUtilities::Point (extends.x, extends.y));
	buttonLabel.resize (BUtilities::Point (extends.x, extends.y));
}

void TextButton::setValue (const double val)
{
	if (val) buttonLabel.setState (BColors::ACTIVE);
	else buttonLabel.setState (BColors::NORMAL);
	Button::setValue (val);
}

Label* TextButton::getLabel () {return &buttonLabel;}

void TextButton::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}
void TextButton::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Button::applyTheme (theme, name);
	buttonLabel.applyTheme (theme, name);
	update ();
}

}
