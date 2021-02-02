/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef VLINE_HPP_
#define VLINE_HPP_

#include "BWidgets/Widget.hpp"

class VLine : public BWidgets::Widget
{
public:
	VLine () : VLine (0, 0, 0, 0, "line") {}

	VLine (const double x, const double y, const double width, const double height, const std::string& name) :
		Widget (x, y, width, height, name),
		bgColors (BWIDGETS_DEFAULT_BGCOLORS)
   	{}

	VLine& operator= (const VLine& that)
	{
		bgColors = that.bgColors;
		Widget::operator= (that);
		return *this;
	}

	virtual BWidgets::Widget* clone () const override {return new VLine (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
		if (fgPtr)
		{
			bgColors = *((BColors::ColorSet*) fgPtr);
			update ();
		}
	}


protected:
	BColors::ColorSet bgColors;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 1) && (getHeight () >= 1))
		{
			// Draw super class widget elements first
			Widget::draw (area);

			cairo_t* cr = cairo_create (widgetSurface_);
			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				double x0 = getXOffset ();
				double y0 = getYOffset ();
				double w = getEffectiveWidth ();
				double h = getEffectiveHeight ();
				BColors::Color color = *bgColors.getColor (getState ());
				color.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);

				cairo_set_line_width (cr, w / 2);
				cairo_move_to (cr, x0 + w / 2, y0);
				cairo_rel_line_to (cr, 0, h);
				cairo_set_source_rgba (cr, CAIRO_RGBA (color));
				cairo_stroke (cr);
				cairo_destroy (cr);
			}
		}
	}
};

#endif /* VLINE_HPP_ */
