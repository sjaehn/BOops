/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef BWIDGETS_MIDITOGGLEBUTTON_HPP_
#define BWIDGETS_MIDITOGGLEBUTTON_HPP_

#include "BWidgets/ToggleButton.hpp"
#include "BWidgets/Label.hpp"

class MidiToggleButton : public BWidgets::ToggleButton
{
public:
	MidiToggleButton () : MidiToggleButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "miditogglebutton", 0.0) {}
	MidiToggleButton (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue = 0.0) :
		ToggleButton (x, y, width, height, name, defaultValue) {}

	virtual Widget* clone () const override {return new MidiToggleButton (*this);}

protected:
	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 6) && (getHeight () >= 6))
		{

			Button::draw (area);

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
				double ext = (w < h ? w * 0.6 : h * 0.6);
				BColors::Color frColor= *bgColors.getColor (getState ());
				frColor.applyBrightness (value ? 2 * BWIDGETS_DEFAULT_ILLUMINATED : 2 * BWIDGETS_DEFAULT_SHADOWED);

				// Symbol
				cairo_set_line_width (cr, BWIDGETS_DEFAULT_BUTTON_BORDER);
                                cairo_set_source_rgba (cr, CAIRO_RGBA (frColor));
                                for (int i = 0; i < 3; ++i) cairo_rectangle (cr, x0 + 0.5 * w - 0.375 * ext + i * 0.25 * ext, y0 + 0.5 * h - 0.45 * ext, 0.25 * ext, 0.9 * ext);
                                cairo_stroke (cr);
                                cairo_set_line_width (cr, 0.0);
                                for (int i = 0; i < 2; ++i)
                                {
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.2 * ext + i * 0.25 * ext, y0 + 0.5 * h - 0.45 * ext, 0.15 * ext, 0.6 * ext);
                                        cairo_fill (cr);
                                }

				cairo_destroy (cr);
			}
		}
	}
};

#endif /* BWIDGETS_MIDITOGGLEBUTTON_HPP_ */
