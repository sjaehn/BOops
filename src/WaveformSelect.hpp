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

#ifndef WAVEFORMSELECT_HPP_
#define WAVEFORMSELECT_HPP_

#include <cmath>
#include "BWidgets/ValueWidget.hpp"

class WaveformSelect : public BWidgets::ValueWidget
{
public:
	WaveformSelect () : WaveformSelect (0, 0, 0, 0, "widget", 0) {}
	WaveformSelect (const double x, const double y, const double width, const double height, const std::string& name, double value = 0.0) :
		ValueWidget (x, y, width, height, name, value)
	{}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		ValueWidget::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr)
		{
			fgColors = *((BColors::ColorSet*) fgPtr);
			update ();
		}

		void* txPtr = theme.getStyle(name, BWIDGETS_KEYWORD_TEXTCOLORS);
		if (txPtr)
		{
			txColors = *((BColors::ColorSet*) txPtr);
			update ();
		}
	}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override
	{
		double x0 = getXOffset ();
		double y0 = getYOffset ();
		double w = getEffectiveWidth ();
		double h = getEffectiveHeight ();

		int col = LIMIT (3.0 * (event->getPosition().x - x0) / w, 0, 2);
		int row = LIMIT (2.0 * (event->getPosition().y - y0) / h, 0, 1);
		int nr = LIMIT (row * 3 + col, 0, 4);
		setValue (0.125 * double (nr));
	}

protected:
	BColors::ColorSet fgColors;
	BColors::ColorSet txColors;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 6) && (getHeight () >= 6))
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

				BColors::Color aColor = *fgColors.getColor (getState ());
				BColors::Color iaColor = *txColors.getColor (getState ());

				cairo_set_line_width (cr, 2.0);

				if (getValue() == 0) cairo_rectangle (cr, x0, y0, 0.28 * w, 0.45 * h);
				cairo_move_to (cr, x0 + 0.04 * w, y0 + 0.225 * h);
				for (double d = 0; d <= 2 * M_PI; d += 0.1 * M_PI) cairo_line_to (cr, x0 + 0.04 * w + 0.2 * w * d / (2.0 * M_PI), y0 + 0.225 * h - 0.175 * h * sin (d));
				if (getValue() == 0) cairo_set_source_rgba (cr, CAIRO_RGBA (aColor));
				else cairo_set_source_rgba (cr, CAIRO_RGBA (iaColor));
				cairo_stroke (cr);

				if (getValue() == 0.125) cairo_rectangle (cr, x0 + 0.36 * w, y0, 0.28 * w, 0.45 * h);
				cairo_move_to (cr, x0 + 0.40 * w, y0 + 0.225 * h);
				cairo_line_to (cr, x0 + 0.45 * w, y0 + 0.05 * h);
				cairo_line_to (cr, x0 + 0.55 * w, y0 + 0.4 * h);
				cairo_line_to (cr, x0 + 0.6 * w, y0 + 0.225 * h);
				if (getValue() == 0.125) cairo_set_source_rgba (cr, CAIRO_RGBA (aColor));
				else cairo_set_source_rgba (cr, CAIRO_RGBA (iaColor));
				cairo_stroke (cr);

				if (getValue() == 0.25) cairo_rectangle (cr, x0 + 0.72 * w, y0, 0.28 * w, 0.45 * h);
				cairo_move_to (cr, x0 + 0.76 * w, y0 + 0.225 * h);
				cairo_line_to (cr, x0 + 0.76 * w, y0 + 0.05 * h);
				cairo_line_to (cr, x0 + 0.86 * w, y0 + 0.05 * h);
				cairo_line_to (cr, x0 + 0.86 * w, y0 + 0.4 * h);
				cairo_line_to (cr, x0 + 0.96 * w, y0 + 0.4 * h);
				cairo_line_to (cr, x0 + 0.96 * w, y0 + 0.225 * h);
				if (getValue() == 0.25) cairo_set_source_rgba (cr, CAIRO_RGBA (aColor));
				else cairo_set_source_rgba (cr, CAIRO_RGBA (iaColor));
				cairo_stroke (cr);

				if (getValue() == 0.375) cairo_rectangle (cr, x0, y0 + 0.55 * h, 0.28 * w, 0.45 * h);
				cairo_move_to (cr, x0 + 0.04 * w, y0 + 0.95 * h);
				cairo_line_to (cr, x0 + 0.24 * w, y0 + 0.6 * h);
				cairo_line_to (cr, x0 + 0.24 * w, y0 + 0.95 * h);
				if (getValue() == 0.375) cairo_set_source_rgba (cr, CAIRO_RGBA (aColor));
				else cairo_set_source_rgba (cr, CAIRO_RGBA (iaColor));
				cairo_stroke (cr);

				if (getValue() == 0.5) cairo_rectangle (cr, x0 + 0.36 * w, y0 + 0.55 * h, 0.28 * w, 0.45 * h);
				cairo_move_to (cr, x0 + 0.4 * w, y0 + 0.95 * h);
				cairo_line_to (cr, x0 + 0.4 * w, y0 + 0.6 * h);
				cairo_line_to (cr, x0 + 0.6 * w, y0 + 0.95 * h);
				if (getValue() == 0.5) cairo_set_source_rgba (cr, CAIRO_RGBA (aColor));
				else cairo_set_source_rgba (cr, CAIRO_RGBA (iaColor));
				cairo_stroke (cr);

				cairo_destroy (cr);
			}
		}
	}
};

#endif /* WAVEFORMSELECT_HPP_ */
