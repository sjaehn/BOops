#ifndef DRAWBUTTON_HPP_
#define DRAWBUTTON_HPP_

#include "BWidgets/cairoplus.h"
#include "BWidgets/BColors.hpp"
#include <cmath>

void drawButton (cairo_t* cr, double x, double y, double width, double height, BColors::Color color)
{
	if ((width <= 0) || (height <= 0)) return;

	// Draw button
	BColors::Color illuminated = color; illuminated.applyBrightness (0.05);
	BColors::Color darkened = color; darkened.applyBrightness (-0.33);
	cairo_pattern_t* pat = cairo_pattern_create_radial (x + width / 2, y + height / 2, 0.125 * width, x + width / 2, y + height / 2, 0.5 * width);

	cairo_pattern_add_color_stop_rgba (pat, 0.0, CAIRO_RGBA (illuminated));
	cairo_pattern_add_color_stop_rgba (pat, 1.0, CAIRO_RGBA (darkened));

	double rad = ((width < 20) || (height < 20) ?  (width < height ? width : height) / 4 : 5);
	cairo_rectangle_rounded (cr, x, y, width, height, rad);
	cairo_set_source (cr, pat);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);
}

void drawButton (cairo_surface_t* surface, double x, double y, double width, double height, BColors::Color color)
{
	cairo_t* cr = cairo_create (surface);
	drawButton (cr, x, y, width, height, color);
	cairo_destroy (cr);
}

#endif /* DRAWBUTTON_HPP_ */
