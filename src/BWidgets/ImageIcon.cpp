/* ImageIcon.cpp
 * Copyright (C) 2019  Sven Jähnichen
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

#include "ImageIcon.hpp"

namespace BWidgets
{
ImageIcon::ImageIcon () : ImageIcon (0.0, 0.0, BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, "icon") {}

ImageIcon::ImageIcon (const double x, const double y, const double width, const double height,
		      const std::string& name) :
		Icon (x, y, width, height, name) {}

ImageIcon::ImageIcon (const double x, const double y, const double width, const double height,
		      const std::string& name, cairo_surface_t* surface) :
		Icon (x, y, width, height, name)
{
	// Fill all standard states with the image from surface
	for (int i = BColors::NORMAL; i < BColors::USER_DEFINED; ++i)
	{
		loadImage (BColors::State (i), surface);
	}
}

ImageIcon::ImageIcon (const double x, const double y, const double width, const double height,
		      const std::string& name, const std::string& filename) :
		Icon (x, y, width, height, name)
{
	// Fill all standard states with the image from filename
	for (unsigned int i = BColors::NORMAL; i < BColors::USER_DEFINED; ++i)
	{
		loadImage (BColors::State (i), filename);
	}
}

ImageIcon::ImageIcon (const double x, const double y, const double width, const double height,
		      const std::string& name, const std::vector<cairo_surface_t*>& surfaces) :
		Icon (x, y, width, height, name)
{
	for (unsigned int i = 0; i < surfaces.size (); ++i) loadImage (BColors::State (i), surfaces[i]);
}

ImageIcon::ImageIcon (const double x, const double y, const double width, const double height,
		      const std::string& name, const std::vector<std::string>& filenames) :
		Icon (x, y, width, height, name)
{
	for (unsigned int i = 0; i < filenames.size (); ++i) loadImage (BColors::State (i), filenames[i]);
}

Widget* ImageIcon::clone () const {return new ImageIcon (*this);}

void ImageIcon::loadImage (BColors::State state, cairo_surface_t* surface)
{
	// Fill empty states with nullptr
	while (state >= iconSurface.size ()) iconSurface.push_back (nullptr);

	// Clear old surface
	if (iconSurface[state] && (cairo_surface_status (iconSurface[state]) == CAIRO_STATUS_SUCCESS))
	{
		cairo_surface_destroy (iconSurface[state]);
		iconSurface[state] = nullptr;
	}

	iconSurface[state] = cairo_image_surface_clone_from_image_surface (surface);
}

void ImageIcon::loadImage (BColors::State state, const std::string& filename)
{
	// Fill empty states with nullptr
	while (state >= iconSurface.size ()) iconSurface.push_back (nullptr);

	// Clear old surface
	if (iconSurface[state] && (cairo_surface_status (iconSurface[state]) == CAIRO_STATUS_SUCCESS))
	{
		cairo_surface_destroy (iconSurface[state]);
		iconSurface[state] = nullptr;
	}

	iconSurface[state] = cairo_image_surface_create_from_png (filename.c_str());
}

}
