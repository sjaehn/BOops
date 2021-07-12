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

#ifndef DRAWSYMBOL_HPP_
#define DRAWSYMBOL_HPP_

#include "BWidgets/cairoplus.h"
#include "BWidgets/BColors.hpp"
#include <cmath>

#ifdef LOCALEFILE
#include LOCALEFILE
#else
#include "Locale_EN.hpp"
#endif

enum SymbolIndex
{
        NOSYMBOL        = -1,
        ADDSYMBOL       = 0,
        CLOSESYMBOL     = 1,
        LEFTSYMBOL      = 2,
        RIGHTSYMBOL     = 3,
        UPSYMBOL        = 4,
        DOWNSYMBOL      = 5,
        PLAYSYMBOL      = 6,
        MIDISYMBOL      = 7,
        SHAPESYMBOL     = 8,
        PATTERNSYMBOL   = 9,
};

const std::string symboltxt[11] =
{
        "",
        BOOPS_LABEL_ADD,
        BOOPS_LABEL_DELETE,
        BOOPS_LABEL_MOVE_FORWARD,
        BOOPS_LABEL_MOVE_BACKWARD,
        BOOPS_LABEL_MOVE_UP,
        BOOPS_LABEL_MOVE_DOWN,
        BOOPS_LABEL_PLAY,
        BOOPS_LABEL_MIDI,
        BOOPS_LABEL_SHAPE_PATTERN, 
        BOOPS_LABEL_SHAPE_PATTERN
};

void drawSymbol (cairo_t* cr, const double x0, const double y0, const double w, const double h, const BColors::Color& color, SymbolIndex symbol = NOSYMBOL)
{
	if ((w <= 0) || (h <= 0)) return;

        const double ext = (w < h ? 0.66 * w : 0.66 * h);

        cairo_set_source_rgba (cr, CAIRO_RGBA (color));

	// Draw button
        switch (symbol)
        {
                case ADDSYMBOL:         cairo_move_to (cr, x0 + w / 2 - ext / 2, y0 + h / 2);
                                        cairo_line_to (cr, x0 + w / 2 + ext / 2, y0 + h / 2);
                                        cairo_move_to (cr, x0 + w / 2, y0 + h / 2 - ext / 2);
                                        cairo_line_to (cr, x0 + w / 2, y0 + h / 2 + ext / 2);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case CLOSESYMBOL:       cairo_move_to (cr, x0 + w / 2 - ext / 2, y0 + h / 2);
                                        cairo_line_to (cr, x0 + w / 2 + ext / 2, y0 + h / 2);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case LEFTSYMBOL:        cairo_move_to (cr, x0 + w / 2 + 0.25 * ext, y0 + h / 2 - ext / 2);
                                        cairo_line_to (cr, x0 + w / 2 - 0.25 * ext, y0 + h / 2);
                                        cairo_line_to (cr, x0 + w / 2 + 0.25 * ext, y0 + h / 2 + ext / 2);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case RIGHTSYMBOL:       cairo_move_to (cr, x0 + w / 2 - 0.25 * ext, y0 + h / 2 - ext / 2);
                                        cairo_line_to (cr, x0 + w / 2 + 0.25 * ext, y0 + h / 2);
                                        cairo_line_to (cr, x0 + w / 2 - 0.25 * ext, y0 + h / 2 + ext / 2);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case UPSYMBOL:          cairo_move_to (cr, x0 + w / 2 - 0.5 * ext, y0 + h / 2 + 0.25 * ext);
                                        cairo_line_to (cr, x0 + w / 2, y0 + h / 2 - 0.25 * ext);
                                        cairo_line_to (cr, x0 + w / 2 + 0.5 * ext, y0 + h / 2 + 0.25 * ext);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case DOWNSYMBOL:        cairo_move_to (cr, x0 + w / 2 - 0.5 * ext, y0 + h / 2 - 0.25 * ext);
                                        cairo_line_to (cr, x0 + w / 2, y0 + h / 2 + 0.25 * ext);
                                        cairo_line_to (cr, x0 + w / 2 + 0.5 * ext, y0 + h / 2 - 0.25 * ext);
                                        cairo_set_line_width (cr, 2.0);
                                        cairo_stroke (cr);
                                        break;

                case PLAYSYMBOL:        cairo_move_to (cr, x0 + w / 2 - 0.5 * ext, y0 + h / 2 - 0.5 * ext);
                                        cairo_line_to (cr, x0 + w / 2 + 0.5 * ext, y0 + h / 2);
                                        cairo_line_to (cr, x0 + w / 2 - 0.5 * ext, y0 + h / 2 + 0.5 * ext);
                                        cairo_close_path (cr);
                                        cairo_set_line_width (cr, 0.0);
                                        cairo_fill (cr);
                                        break;

                case MIDISYMBOL:        cairo_set_line_width (cr, 1.0);
                                        for (int i = 0; i < 3; ++i)
                                        {
                                                cairo_rectangle (cr, x0 + 0.5 * w - 0.375 * ext + i * 0.25 * ext, y0 + 0.5 * h - 0.45 * ext, 0.25 * ext, 0.9 * ext);
                                        }
                                        cairo_stroke (cr);
                                        cairo_set_line_width (cr, 0.0);
                                        for (int i = 0; i < 2; ++i)
                                        {
                                                cairo_rectangle (cr, x0 + 0.5 * w - 0.2 * ext + i * 0.25 * ext, y0 + 0.5 * h - 0.45 * ext, 0.15 * ext, 0.6 * ext);
                                                cairo_fill (cr);
                                        }
                                        break;

                case SHAPESYMBOL:       cairo_set_line_width (cr, 2.0);
                                        cairo_move_to (cr, x0 + 0.5 * w - 0.5 * ext, y0 + 0.5 * h + 0.3 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w - 0.4 * ext, y0 + 0.5 * h + 0.5 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w - 0.2 * ext, y0 + 0.5 * h + 0.5 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w - 0.1 * ext, y0 + 0.5 * h + 0.3 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w + 0.0 * ext, y0 + 0.5 * h - 0.1 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w + 0.1 * ext, y0 + 0.5 * h - 0.3 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w + 0.2 * ext, y0 + 0.5 * h - 0.5 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w + 0.4 * ext, y0 + 0.5 * h - 0.5 * ext);
                                        cairo_line_to (cr, x0 + 0.5 * w + 0.5 * ext, y0 + 0.5 * h - 0.3 * ext);
                                        cairo_stroke (cr);
                                        break;

                case PATTERNSYMBOL:     cairo_set_line_width (cr, 0.0);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.5 * ext, y0 + 0.5 * h - 0.5 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.1 * ext, y0 + 0.5 * h - 0.5 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w + 0.3 * ext, y0 + 0.5 * h - 0.5 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.3 * ext, y0 + 0.5 * h - 0.3 * ext, 0.4 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.5 * ext, y0 + 0.5 * h - 0.1 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.3 * ext, y0 + 0.5 * h + 0.1 * ext, 0.4 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.5 * ext, y0 + 0.5 * h + 0.3 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w - 0.1 * ext, y0 + 0.5 * h + 0.3 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        cairo_rectangle (cr, x0 + 0.5 * w + 0.3 * ext, y0 + 0.5 * h + 0.3 * ext, 0.2 * w, 0.1 * h);
                                        cairo_fill (cr);
                                        break;

                default:                break;
        }
}

void drawSymbol (cairo_surface_t* surface, const double x, const double y, const double width, const double height, const BColors::Color& color, SymbolIndex symbol = NOSYMBOL)
{
	cairo_t* cr = cairo_create (surface);
	drawSymbol (cr, x, y, width, height, color, symbol);
	cairo_destroy (cr);
}

#endif /* DRAWSYMBOL_HPP_ */
