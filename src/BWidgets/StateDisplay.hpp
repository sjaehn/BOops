/* StateDisplay.hpp
 * Copyright (C) 2019  Sven Jähnichen
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

#ifndef BWIDGETS_STATEDISPLAY_HPP_
#define BWIDGETS_STATEDISPLAY_HPP_

#include "Display.hpp"

namespace BWidgets
{

/**
 * Class BWidgets::StateDisplay
 *
 * Display widget that displays only child widgets of the same state.
 */
class StateDisplay : public Display
{
public:
	StateDisplay ();
	StateDisplay (const double x, const double y, const double width, const double height,
			const std::string& text);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

protected:
	virtual bool filter (Widget* widget) override;
};

}

#endif /* BWIDGETS_STATEDISPLAY_HPP_ */
