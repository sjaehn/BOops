/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019, 2020 by Sven Jähnichen
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

#ifndef MONITORWIDGET_HPP_
#define MONITORWIDGET_HPP_

#include "BWidgets/Widget.hpp"
#include "Definitions.hpp"
#include <cmath>

class MonitorWidget : public BWidgets::Widget
{
public:
        MonitorWidget () : MonitorWidget (0, 0, 0, 0, "monitor") {}

        MonitorWidget (const double x, const double y, const double width, const double height, const std::string& name) :
                Widget (x, y, width, height, name),
                fgColors (BColors::whites), zoom (0.5)
        {
                clear ();
                setClickable (false);
                setDraggable (false);
                setScrollable (false);
                setFocusable (false);
        }

        void clear () {data.fill (0.0f);}

        void addData (const unsigned int pos, const unsigned int size, const float* data)
        {
                for (unsigned int i = 0; i < size; ++i) this->data[(i + pos) % WAVEFORMSIZE] = data[i];
        }

        void setZoom (const double factor)
        {
                zoom = factor;
                update ();
        }

        double getZoom () const {return zoom;}

        void redrawRange (const unsigned int start, const unsigned int size)
        {
                unsigned int s = LIMIT (int (start) - 1, 0, WAVEFORMSIZE - 1);
                unsigned int e = LIMIT (start + size - 1, 0, WAVEFORMSIZE - 1);
                double xabs = getAbsolutePosition().x;
                double yabs = getAbsolutePosition().y;
                double x1 = getWidth() * s / (WAVEFORMSIZE - 1);
                double w = getWidth() * (e - s) / (WAVEFORMSIZE - 1);
                drawData (s, e);
                if (isVisible ()) postRedisplay (BUtilities::RectArea (floor (xabs + x1), yabs, ceil (w), getHeight()));

                if (start + size > WAVEFORMSIZE)
                {
                        unsigned int e2 = (start + size - 1) % WAVEFORMSIZE;
                        double w2 = getWidth() * e2 / (WAVEFORMSIZE - 1);
                        drawData (0, e2);
                        if (isVisible ()) postRedisplay (BUtilities::RectArea (floor (xabs), yabs, ceil (w2), getHeight()));
                }
        }

        virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
        {
        	Widget::applyTheme (theme, name);

        	void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
        	if (fgPtr)
        	{
        		fgColors = *((BColors::ColorSet*) fgPtr);
        		update ();
        	}
        }

        virtual void onWheelScrolled (BEvents::WheelEvent* event) override
        {
        	double step = getZoom() * 0.01;
        	setZoom (getZoom() + event->getDelta ().y * step);
        }

protected:
        void drawData (const unsigned int start, const unsigned int end)
        {
                if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

                BColors::Color col = *fgColors.getColor (getState ());
        	cairo_t* cr = cairo_create (widgetSurface_);

        	if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
        	{
        		// Limit cairo-drawing area
                        cairo_set_line_width (cr, 0);
                        double x0 = ceil (getWidth() * start / (WAVEFORMSIZE - 1));
                        double x1 = floor (getWidth() * end / (WAVEFORMSIZE - 1));
        		cairo_rectangle (cr, x0, 0, x1 - x0, getHeight());
        		cairo_clip (cr);

        		cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
        		cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        		cairo_paint (cr);

                        cairo_set_line_width (cr, 1);
                        cairo_move_to (cr, getWidth() * double (start) / (WAVEFORMSIZE - 1), getHeight() * (0.5  - (0.48 * data[start] / zoom)));
                        for (int i = start + 1; i <= int (end); ++i)
                        {
                                cairo_line_to (cr, getWidth() * double (i) / (WAVEFORMSIZE - 1), getHeight() * (0.5  - (0.48 * data[i] / zoom)));
                        }
                        cairo_set_source_rgba (cr, CAIRO_RGBA (col));
                        cairo_stroke (cr);

                        cairo_destroy (cr);
                }
        }

        virtual void draw (const BUtilities::RectArea& area) override
        {
                drawData (0, WAVEFORMSIZE - 1);
        }

        std::array<float, WAVEFORMSIZE> data;
        BColors::ColorSet fgColors;
        double zoom;
};

#endif /* MONITORWIDGET_HPP_ */
