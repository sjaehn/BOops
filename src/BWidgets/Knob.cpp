/* Knob.cpp
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

#include "Knob.hpp"

namespace BWidgets
{
Knob::Knob () : Knob (0.0, 0.0, BWIDGETS_DEFAULT_KNOB_WIDTH, BWIDGETS_DEFAULT_KNOB_HEIGHT, BWIDGETS_DEFAULT_KNOB_DEPTH, "knob") {}

Knob::Knob (const double  x, const double y, const double width, const double height, const double depth, const std::string& name) :
		Widget (x, y, width, height, name),
		knobDepth (depth), bgColors (BWIDGETS_DEFAULT_BGCOLORS)
{
	setClickable (true);
	setDraggable (true);
	setScrollable (true);
}

Knob::Knob (const Knob& that) : Widget (that), knobDepth (that.knobDepth), bgColors (that.bgColors){}

Knob::~Knob () {}

Knob& Knob::operator= (const Knob& that)
{
	knobDepth = that.knobDepth;
	bgColors = that.bgColors;
	Widget::operator= (that);

	return *this;
}

Widget* Knob::clone () const {return new Knob (*this);}

void Knob::setDepth (const double depth)
{
	knobDepth = depth;
	update ();
}

double Knob::getDepth () const {return knobDepth;}

void Knob::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void Knob::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);

	void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
	if (bgPtr)
	{
		bgColors = *((BColors::ColorSet*) bgPtr);
		update ();
	}

}

void Knob::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	// Draw super class widget elements first
	Widget::draw (area);

	double heff = getEffectiveHeight ();
	double weff = getEffectiveWidth ();

	// Draw knob
	// only if minimum requirements satisfied
	if ((getHeight () >= 1) && (getWidth () >= 1))
	{
		cairo_surface_clear (widgetSurface_);
		cairo_t* cr = cairo_create (widgetSurface_);

		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			cairo_pattern_t* pat;

			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
			cairo_clip (cr);

			// Calculate aspect ratios first
			double size = (heff < weff ? heff - 2 * knobDepth : weff - 2 * knobDepth);
			double x0 = weff / 2 - size / 2;
			double y0 = heff / 2 - size / 2;

			// Colors uses within this method
			BColors::Color cLo = *bgColors.getColor (getState ()); cLo.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color cHi = *bgColors.getColor (getState ()); cHi.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
			BColors::Color cMid = *bgColors.getColor (getState ()); cMid.applyBrightness ((BWIDGETS_DEFAULT_ILLUMINATED + BWIDGETS_DEFAULT_NORMALLIGHTED) / 2);
			BColors::Color cSh = *bgColors.getColor (getState ()); cSh.applyBrightness (BWIDGETS_DEFAULT_SHADOWED);

			cairo_set_line_width (cr, 0.0);

			// Knob top
			pat = cairo_pattern_create_radial (x0 + 0.75 * size - knobDepth, y0 + 0.75 * size - knobDepth, 0.1 * size,
											   x0 + 0.5 * size - knobDepth, y0 + 0.5 * size - knobDepth, 0.75 * size);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_arc (cr,  x0 + 0.5 * size - knobDepth, y0 + 0.5 * size - knobDepth, size / 2, 0, 2 * M_PI);
				cairo_close_path (cr);
				cairo_pattern_add_color_stop_rgba (pat, 0, cMid.getRed (), cMid.getGreen (), cMid.getBlue (), cMid.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, cLo.getRed (), cLo.getGreen (), cLo.getBlue (), cLo.getAlpha ());
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}

			// Knob side
			pat = cairo_pattern_create_linear (x0 - knobDepth, y0, x0 + size, y0);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, cMid.getRed (), cMid.getGreen (), cMid.getBlue (), cMid.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0.825, cHi.getRed (), cHi.getGreen (), cHi.getBlue (), cHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, cMid.getRed (), cMid.getGreen (), cMid.getBlue (), cMid.getAlpha ());
				cairo_arc (cr,  x0 + 0.5 * size - knobDepth, y0 + 0.5 * size - knobDepth, size / 2, -0.25 * M_PI, 0.75 * M_PI);
				cairo_line_to (cr, x0, y0 + 0.5 * size);
				cairo_arc_negative (cr, x0 + 0.5 * size, y0 + 0.5 * size, size / 2, 0.75 * M_PI, -0.25 * M_PI);
				cairo_close_path (cr);
				cairo_set_line_width (cr, 0.5);
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}

			// Knob edge
			pat = cairo_pattern_create_linear
			(
				x0 + 0.5 * size * (1 + cos (0.25 * M_PI)) - knobDepth,
				y0 + 0.5 * size * (1 + sin (0.25 * M_PI)) - knobDepth,
				x0 + 0.5 * size * (1 - cos (0.25 * M_PI)) - knobDepth,
				y0 + 0.5 * size * (1 - sin (0.25 * M_PI)) - knobDepth
			);

			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, cHi.getRed (), cHi.getGreen (), cHi.getBlue (), cHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, cSh.getRed (), cSh.getGreen (), cSh.getBlue (), cSh.getAlpha ());
				cairo_arc (cr,  x0 + 0.5 * size - knobDepth, y0 + 0.5 * size - knobDepth, size / 2, 0, 2 * M_PI);
				cairo_set_line_width (cr, 0.2 * knobDepth);
				cairo_set_source (cr, pat);
				cairo_stroke (cr);
				cairo_pattern_destroy (pat);
			}
		}

		cairo_destroy (cr);
	}
}

}
