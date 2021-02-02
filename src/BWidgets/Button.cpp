/* Button.cpp
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

#include "Button.hpp"

namespace BWidgets
{
Button::Button () : Button (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "button", 0.0) {}

Button::Button (const double x, const double y, const double width, const double height, const std::string& name, double defaultValue) :
		ValueWidget (x, y, width, height, name, defaultValue),
		bgColors (BWIDGETS_DEFAULT_BGCOLORS)
{
	setClickable (true);
}

Button::Button (const Button& that) : ValueWidget (that), bgColors (that.bgColors) {}

Button:: ~Button () {}

Button& Button::operator= (const Button& that)
{
	bgColors = that.bgColors;
	ValueWidget::operator= (that);
	return *this;
}

Widget* Button::clone () const {return new Button (*this);}

void Button::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void Button::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);

	void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
	if (bgPtr)
	{
		bgColors = *((BColors::ColorSet*) bgPtr);
		update ();
	}
}

void Button::onButtonPressed (BEvents::PointerEvent* event)
{
	setValue (1.0);
	Widget::cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
}

void Button::onButtonReleased (BEvents::PointerEvent* event)
{
	setValue (0.0);
	Widget::cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);
}

void Button::draw (const BUtilities::RectArea& area)
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
			BColors::Color butColor = *bgColors.getColor (getState ()); butColor.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color frColor= *bgColors.getColor (getState ());

			if (value) frColor.applyBrightness (2 * BWIDGETS_DEFAULT_ILLUMINATED);
			else frColor.applyBrightness (2 * BWIDGETS_DEFAULT_SHADOWED);

			cairo_set_line_width (cr, 0.0);
			cairo_set_source_rgba (cr, CAIRO_RGBA (butColor));
			cairo_rectangle_rounded (cr, x0, y0, w, h, BWIDGETS_DEFAULT_BUTTON_RAD, 0b1111);
			cairo_fill_preserve (cr);

			cairo_set_line_width (cr, BWIDGETS_DEFAULT_BUTTON_BORDER);
			cairo_set_source_rgba (cr, CAIRO_RGBA (frColor));
			cairo_stroke (cr);

		}
		cairo_destroy (cr);
	}
}

}
