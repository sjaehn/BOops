/* HScale.cpp
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

#include "HScale.hpp"

namespace BWidgets
{
HScale::HScale () : HScale (0.0, 0.0, BWIDGETS_DEFAULT_HSCALE_WIDTH, BWIDGETS_DEFAULT_HSCALE_HEIGHT, "hscale",
		  	  	  	  	  	   BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP) {}

HScale::HScale (const double  x, const double y, const double width, const double height, const std::string& name,
				  const double value, const double min, const double max, const double step) :
		RangeWidget (x, y, width, height, name, value, min, max, step),
		fgColors (BWIDGETS_DEFAULT_FGCOLORS), bgColors (BWIDGETS_DEFAULT_BGCOLORS),
		scaleArea (0, 0, width, height), scaleXValue (0)
{
	setClickable (true);
	setDraggable (true);
	setScrollable (true);
}

HScale::HScale (const HScale& that) :
		RangeWidget (that), fgColors (that.fgColors), bgColors (that.bgColors),
		scaleArea (that.scaleArea), scaleXValue (that.scaleXValue)
{}

HScale& HScale::operator= (const HScale& that)
{
	fgColors = that.fgColors;
	bgColors = that.bgColors;
	scaleArea = that.scaleArea;
	scaleXValue = that.scaleXValue;
	RangeWidget::operator= (that);

	return *this;
}

Widget* HScale::clone () const {return new HScale (*this);}

void HScale::update ()
{
	updateCoords ();
	Widget::update();
}

void HScale::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void HScale::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);

	// Foreground colors (scale)
	void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
	if (fgPtr) fgColors = *((BColors::ColorSet*) fgPtr);

	// Background colors (scale background, knob)
	void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
	if (bgPtr) bgColors = *((BColors::ColorSet*) bgPtr);

	if (fgPtr || bgPtr) update ();

}

void HScale::onButtonPressed (BEvents::PointerEvent* event)
{
	if
	(
		main_ &&
		isVisible () &&
		(getHeight () >= 1) &&
		(getWidth () >= 1) &&
		(scaleArea.getWidth () > 0) &&
		(event->getButton() == BDevices::LEFT_BUTTON)
	)
	{
		double min = getMin ();
		double max = getMax ();

		// Use pointer coords directly if hardSetable , otherwise apply only
		// X movement (drag mode)
		if (hardChangeable)
		{
			double frac = (event->getPosition ().x - scaleArea.getX ()) / scaleArea.getWidth ();
			if (getStep () < 0) frac = 1 - frac;
			double hardValue = min + frac * (max - min);
			softValue = 0;
			setValue (hardValue);
		}
		else
		{
			if (min != max)
			{
				double deltaFrac = event->getDelta ().x / scaleArea.getWidth ();
				if (getStep () < 0) deltaFrac = -deltaFrac;
				softValue += deltaFrac * (max - min);
				setValue (getValue() + softValue);
			}
		}
	}
}

void HScale::onButtonReleased (BEvents::PointerEvent* event) {softValue = 0.0;}

void HScale::onPointerDragged (BEvents::PointerEvent* event) {onButtonPressed (event);}

void HScale::onWheelScrolled (BEvents::WheelEvent* event)
{
	double min = getMin ();
	double max = getMax ();

	if (min != max)
	{
		double step = (getStep () != 0 ? getStep () : (max - min) / scaleArea.getWidth ());
		setValue (getValue() + event->getDelta ().y * step);
	}
}

void HScale::updateCoords ()
{
	scaleArea = BUtilities::RectArea (getXOffset (), getYOffset (), getEffectiveWidth (), getEffectiveHeight ());
	scaleXValue = scaleArea.getX() + getRelativeValue () * scaleArea.getWidth ();
}

void HScale::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	// Draw super class widget elements first
	Widget::draw (area);

	// Draw scale only if it is not a null widget
	if ((scaleArea.getHeight() >= 1) && (scaleArea.getWidth() >= 1))
	{
		cairo_surface_clear (widgetSurface_);
		cairo_t* cr = cairo_create (widgetSurface_);

		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			cairo_pattern_t* pat = nullptr;

			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
			cairo_clip (cr);

			// Calculate aspect ratios first
			double h = scaleArea.getHeight ();
			double w = scaleArea.getWidth ();
			double x1 = scaleArea.getX(); double y1 = scaleArea.getY();	// Top left
			double x2 = scaleXValue; double y2 = y1 + h; 			// Value line bottom
			double x3 = scaleXValue; double y3 = y1;			// Value line top
			double x4 = x1 + w; double y4 = y2; 				// Bottom right

			// Colors uses within this method
			BColors::Color fgHi = *fgColors.getColor (getState ()); fgHi.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
			BColors::Color fgMid = *fgColors.getColor (getState ()); fgMid.applyBrightness ((BWIDGETS_DEFAULT_ILLUMINATED + BWIDGETS_DEFAULT_NORMALLIGHTED) / 2);
			BColors::Color fgLo = *fgColors.getColor (getState ()); fgLo.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color bgLo = *bgColors.getColor (getState ()); bgLo.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color bgHi = *bgColors.getColor (getState ()); bgHi.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
			BColors::Color bgMid = *bgColors.getColor (getState ()); bgMid.applyBrightness ((BWIDGETS_DEFAULT_ILLUMINATED + BWIDGETS_DEFAULT_NORMALLIGHTED) / 2);
			BColors::Color bgSh = *bgColors.getColor (getState ()); bgSh.applyBrightness (BWIDGETS_DEFAULT_SHADOWED);

			cairo_set_line_width (cr, 0.0);
			cairo_rectangle_rounded (cr, x1, y1, x4 - x1, y4 - y1, (y4 - y1) / 2);
			cairo_clip (cr);

			// Frame background
			pat = cairo_pattern_create_linear (x4, y4, x1, y1);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, bgLo.getRed (), bgLo.getGreen (), bgLo.getBlue (), bgLo.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_rectangle_rounded (cr, x1, y1, x4 - x1, y4 - y1, (y4 - y1) / 2);
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}
			cairo_rectangle_rounded (cr, x1 + BWIDGETS_DEFAULT_HSCALE_DEPTH, y1 + BWIDGETS_DEFAULT_HSCALE_DEPTH, x4 - x1, y4 - y1, (y4 - y1) / 2);
			cairo_set_source_rgba (cr, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
			cairo_fill (cr);

			// Scale active
			pat = cairo_pattern_create_linear (x3, y3, x2, y2);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 1, fgLo.getRed (), fgLo.getGreen (), fgLo.getBlue (), fgLo.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0.75, fgHi.getRed (), fgHi.getGreen (), fgHi.getBlue (), fgHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0, fgLo.getRed (), fgLo.getGreen (), fgLo.getBlue (), fgLo.getAlpha ());
				if (getStep () < 0)
				{
					cairo_rectangle_rounded (cr, x3 + 0.5 * BWIDGETS_DEFAULT_HSCALE_DEPTH,
											 y3 + 0.5 * BWIDGETS_DEFAULT_HSCALE_DEPTH, x4 - x3, y4 - y3, (y4 - y3) / 2, 0b0110);
				}
				else
				{
					cairo_rectangle_rounded (cr, x1 + 0.5 * BWIDGETS_DEFAULT_HSCALE_DEPTH,
											 y1 + 0.5 * BWIDGETS_DEFAULT_HSCALE_DEPTH, x2 - x1, y2 - y1, (y2 - y1) / 2, 0b1001);
				}
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}

			//Frame
			pat = cairo_pattern_create_linear (x4, y4, x1, y1);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, bgLo.getRed (), bgLo.getGreen (), bgLo.getBlue (), bgLo.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_rectangle_rounded (cr, x1, y1, x4 - x1, y4 - y1, (y4 - y1) / 2);
				cairo_set_source (cr, pat);
				cairo_set_line_width (cr, 0.2 * BWIDGETS_DEFAULT_HSCALE_DEPTH);
				cairo_stroke (cr);
				cairo_pattern_destroy (pat);
			}
		}
		cairo_destroy (cr);
	}
}

}
