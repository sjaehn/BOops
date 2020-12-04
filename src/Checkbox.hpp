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

#ifndef CHECKBOX_HPP_
#define CHECKBOX_HPP_

#include "BWidgets/ToggleButton.hpp"

class Checkbox : public BWidgets::ToggleButton
{
public:
	Checkbox () : Checkbox (0, 0, 0, 0, "checkbox", 0) {}
	Checkbox (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue = 0.0) :
		ToggleButton (x, y, width, height, name, defaultValue),
		fgColors (BWIDGETS_DEFAULT_FGCOLORS)
	{}

	virtual Widget* clone () const override {return new Checkbox (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		ToggleButton::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr)
		{
			fgColors = *((BColors::ColorSet*) fgPtr);
			update ();
		}
	}

protected:
	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getEffectiveWidth () >= 6) && (getEffectiveHeight () >= 6))
		{
			// Draw super class widget elements first
			Widget::draw (area);

			cairo_t* cr = cairo_create (widgetSurface_);
			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				const double x0 = getXOffset ();
				const double y0 = getYOffset ();
				const double w = getEffectiveWidth ();
				const double h = getEffectiveHeight ();
				const double d = (w < h ? w : h) - 2;

				const BColors::Color bg = *bgColors.getColor (getState ());
				const BColors::Color fg= *fgColors.getColor (getState ());

				cairo_set_line_width (cr, 2.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (bg));
				cairo_rectangle (cr, x0 + 0.5 * w - 0.5 * d, y0 + 0.5 * h - 0.5 * d, d, d);
				cairo_stroke (cr);

				if (getValue() != 0.0)
				{
					cairo_set_line_width (cr, 2.0);
					cairo_set_source_rgba (cr, CAIRO_RGBA (fg));
					cairo_move_to (cr, x0 + 0.5 * w - 0.45 * d, y0 + 0.5 * h);
					cairo_line_to (cr, x0 + 0.5 * w - 0.15 * d, y0 + 0.5 * h + 0.45 * d);
					cairo_line_to (cr, x0 + 0.5 * w + 0.45 * d, y0 + 0.5 * h - 0.45 * d);
					cairo_stroke (cr);
				}
			}

			cairo_destroy (cr);
		}
	}

	BColors::ColorSet fgColors;

};

#endif /* CHECKBOX_HPP_ */
