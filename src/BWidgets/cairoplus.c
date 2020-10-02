/* cairoplus.c
 * Copyright (C) 2018  Sven Jähnichen
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

#include "cairoplus.h"

#ifndef PI
#define PI 3.14159265
#endif

#ifndef FABS
#define FABS(x) (x < 0 ? -x : x)
#endif

void cairo_rectangle_rounded (cairo_t* cr, double x, double y, double width, double height, double radius, uint8_t corners)
{
	if (radius == 0.0)
	{
		cairo_rectangle (cr, x, y, width, height);
	}
	else
	{
		radius = FABS (radius);
		if (width < 0)
		{
			width = FABS (width);
			x = x - width;
		}
		if (height < 0)
		{
			height = FABS (height);
			y = y - height;
		}

		cairo_new_sub_path (cr);

		if (corners & 1) cairo_arc (cr, x + radius, y + radius, radius, -PI, -0.5 * PI);
		else cairo_move_to (cr, x, y);

		if (corners & 2) cairo_arc (cr, x + width - radius, y + radius, radius, -0.5 * PI, 0);
		else cairo_line_to (cr, x + width, y);

		if (corners & 4) cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, 0.5 * PI);
		else cairo_line_to (cr, x + width, y + height);

		if (corners & 8) cairo_arc (cr, x + radius, y + height - radius, radius, 0.5 * PI, PI);
		else cairo_line_to (cr, x, y + height);

		cairo_close_path (cr);
	}
}

cairo_surface_t* cairo_image_surface_clone_from_image_surface (cairo_surface_t* sourceSurface)
{
	cairo_surface_t* targetSurface = NULL;

	if (sourceSurface && (cairo_surface_status (sourceSurface) == CAIRO_STATUS_SUCCESS))
	{
		cairo_format_t format = cairo_image_surface_get_format (sourceSurface);
		int width = cairo_image_surface_get_width (sourceSurface);
		int height = cairo_image_surface_get_height (sourceSurface);
		targetSurface = cairo_image_surface_create (format, width, height);
		cairo_t* cr = cairo_create (targetSurface);
		if (targetSurface && (cairo_surface_status (targetSurface) == CAIRO_STATUS_SUCCESS))
		{
			if (cr && (cairo_status (cr) == CAIRO_STATUS_SUCCESS))
			{
				cairo_set_source_surface (cr, sourceSurface, 0, 0);
				cairo_paint (cr);
				cairo_destroy (cr);
			}
		}
	}

	return targetSurface;
}

void cairo_surface_clear (cairo_surface_t* surface)
{
	cairo_t* cr = cairo_create (surface);
	if (cr && (cairo_status (cr) == CAIRO_STATUS_SUCCESS))
	{
		cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
		cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint (cr);
		cairo_destroy (cr);
	}
}

char cairo_nil_text[1] = "";

char* cairo_create_text_fitted (cairo_t* cr, double width, cairo_text_decorations decorations, char* text)
{
	size_t text_len = strlen (text);
	if (text_len)
	{
		char* output = (char*) malloc (text_len + 1);
		if (output)
		{
			if (cr && (! cairo_status (cr)))
			{
				cairo_save (cr);

				cairo_text_extents_t ext;
				cairo_select_font_face (cr, decorations.family, decorations.slant, decorations.weight);
				cairo_set_font_size (cr, decorations.size);

				// First look for \n (or end of string)
				size_t pos = strcspn (text, "\n");

				// Text stupidly starts with \n?
				if (pos == 0) output[0] = '\0';

				else
				{
					strncpy (output, text, pos);
					output[pos] = '\0';
					cairo_text_extents (cr, output, &ext);

					// Text too long? Scan for spaces.
					if (ext.width > width)
					{
						while ((ext.width > width) && strrchr(output, ' '))
						{
							pos = (size_t) (strrchr(output, ' ') - output);
							output[pos] = '\0';
							cairo_text_extents (cr, output, &ext);
						}

						// Still too long? Hard break within a word. At least one char.
						if (ext.width > width)
						{
							while ((ext.width > width) && (strlen (output) > 1))
							{
								pos = strlen (output) - 1;
								output[pos] = '\0';
								cairo_text_extents (cr, output, &ext);
							}

							// Fix: Don't loose the first char after line break.
							pos = pos - 1;
						}
					}
				}

				cairo_restore (cr);

				if (pos < text_len) memmove (text, text + pos + 1, text_len - pos);
				else text[0] = '\0';
				return output;
			}
			else free (output);
		}
	}

	return cairo_nil_text;
}

void cairo_text_destroy (char* text)
{
	if (text && (text != cairo_nil_text)) free (text);
}
