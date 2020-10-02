/* ToggleButton.cpp
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

#include "ToggleButton.hpp"

namespace BWidgets
{
ToggleButton::ToggleButton () : ToggleButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "togglebutton", 0.0) {}

ToggleButton::ToggleButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue) :
		Button (x, y, width, height, name, defaultValue) {}

Widget* ToggleButton::clone () const {return new ToggleButton (*this);}

void ToggleButton::onButtonPressed (BEvents::PointerEvent* event)
{
	if (getValue ()) setValue (0.0);
	else setValue (1.0);

	Widget::cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
}

void ToggleButton::onButtonReleased (BEvents::PointerEvent* event)
{
	Widget::cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);
}

}
