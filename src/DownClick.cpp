/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "DownClick.hpp"

DownClick::DownClick () : DownClick (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "togglebutton", 0.0) {}

DownClick::DownClick (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue) :
			Button (x, y, width, height, name, defaultValue) {}

void DownClick::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	if ((area.getWidth() >= 6) && (area.getHeight() >= 6))
	{
		// Draw super class widget elements first
		Widget::draw (area);

		cairo_t* cr = cairo_create (widgetSurface_);
		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX(), area.getY(), area.getWidth(), area.getHeight());
			cairo_clip (cr);

			double x0 = getXOffset ();
			double y0 = getYOffset ();
			double w = getEffectiveWidth ();
			double h = getEffectiveHeight ();
			BColors::Color butColor = *bgColors.getColor (value == 1 ? BColors::ACTIVE : BColors::NORMAL);

			cairo_move_to (cr, x0, y0 + 0.25 * h);
			cairo_line_to (cr, x0 + 0.5 * w, y0 + 0.75 * h);
			cairo_line_to (cr, x0 + w, y0 + 0.25 * h);

			cairo_set_line_width (cr, 2);
			cairo_set_source_rgba (cr, CAIRO_RGBA (butColor));
			cairo_stroke (cr);

			cairo_destroy (cr);
		}
	}
}
