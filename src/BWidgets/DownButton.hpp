/* DownButton.hpp
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

#ifndef BWIDGETS_DOWNBUTTON_HPP_
#define BWIDGETS_DOWNBUTTON_HPP_

#include "Button.hpp"
#include "Label.hpp"

namespace BWidgets
{
/**
 * Class BWidgets::DownButton
 *
 * Text button widget. Is is a BWidgets::Button and thus a
 * BWidgets::ValueWidget having two conditions: on (value != 0) or off
 * (value == 0)
 */
class DownButton : public Button
{
public:
	DownButton ();
	DownButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue = 0.0);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;
};

}

#endif /* BWIDGETS_TOGGLEBUTTON_HPP_ */
