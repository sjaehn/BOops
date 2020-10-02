/* DrawingSurface.hpp
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

#ifndef DRAWINGSURFACE_HPP_
#define DRAWINGSURFACE_HPP_

#include "Widget.hpp"

namespace BWidgets
{
/**
 * Class BWidgets::DrawingSurface
 *
 * Basic button widget drawing a button. Is is a BWidgets::ValueWidget having
 * two conditions: on (value != 0) or off (value == 0). The Widget is clickable
 * by default.
 */
class DrawingSurface : public Widget
{
public:
	DrawingSurface ();
	DrawingSurface (const double x, const double y, const double width, const double height, const std::string& name);

	/**
	 * Creates a new (orphan) drawing surface widget and copies the widget
	 * properties from a source drawing surface widget. This method doesn't
	 * copy any parent or child widgets.
	 * @param that Source drawing surface widget
	 */
	DrawingSurface (const DrawingSurface& that);

	~DrawingSurface ();

	/**
	 * Assignment. Copies the widget properties from a source widget and keeps
	 * its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param that Source widget
	 */
	DrawingSurface& operator= (const DrawingSurface& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Gets (a pointer to) the Cairo surface for direct drawing provided by the
	 * widget.
	 * @return Cairo surface
	 */
	cairo_surface_t* getDrawingSurface ();

	/**
	 * Resizes the widget and the drawing surface, redraw and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	 * @param width New widgets width
	 */
	virtual void setWidth (const double width) override;

	/**
	 * Resizes the widget and the drawing surface, redraw and emits a
	 * BEvents::ExposeEvent if the widget is visible.
	* @param height New widgets height
	 */
	virtual void setHeight (const double height) override;

	/**
	 * Resizes the widget and the drawing surface,
	 * redraw and emits a BEvents::ExposeEvent if the
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
	 * (Re-)Defines the border of the widget and resizes the drawing surface.
	 *  Redraws widget and emits a BEvents::ExposeEvent if the widget is
	 *  visible.
	 * @param border New widgets border
	 */
	virtual void setBorder (const BStyles::Border& border) override;

protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	cairo_surface_t* drawingSurface;
};

}

#endif /* DRAWINGSURFACE_HPP_ */
