/* ImageIcon.hpp
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

#ifndef ICON_HPP_
#define ICON_HPP_

#include "Widget.hpp"

namespace BWidgets
{
/**
 * Class BWidgets::Icon
 *
 * Widget displaying an icon.
 */
class Icon : public Widget
{
public:
	Icon ();
	Icon (const double x, const double y, const double width, const double height, const std::string& name);

	/**
	 * Creates a new (orphan) image icon widget and copies the widget
	 * properties from a source image icon widget. This method doesn't
	 * copy any parent or child widgets.
	 * @param that Source drawing surface widget
	 */
	Icon (const Icon& that);

	~Icon ();

	/**
	 * Assignment. Copies the widget properties from a source widget and keeps
	 * its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param that Source widget
	 */
	Icon& operator= (const Icon& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	cairo_surface_t* getIconSurface (BColors::State state) const;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	std::vector<cairo_surface_t*> iconSurface;
};

}

#endif /* ICON_HPP_ */
