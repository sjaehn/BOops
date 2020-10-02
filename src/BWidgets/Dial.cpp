/* Dial.cpp
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

#include "Dial.hpp"
#include "../BUtilities/to_string.hpp"

namespace BWidgets
{
Dial::Dial () : Dial (0.0, 0.0, BWIDGETS_DEFAULT_DIAL_WIDTH, BWIDGETS_DEFAULT_DIAL_HEIGHT, "dial",
					  BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP) {}

Dial::Dial (const double x, const double y, const double width, const double height, const std::string& name,
			const double value, const double min, const double max, const double step) :
		RangeWidget (x, y, width, height, name, value, min, max, step),
		Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
			std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
		dialCenter (width / 2, height / 2), dialRadius (width < height ? width / 2 : height / 2),
		knob ((1 - BWIDGETS_DEFAULT_DIAL_KNOB_SIZE) / 2 * width, (1 - BWIDGETS_DEFAULT_DIAL_KNOB_SIZE) / 2 * height,
			   BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * width, BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * height, BWIDGETS_DEFAULT_KNOB_DEPTH,
			   name),
		dot (0, 0, (width < height ? BWIDGETS_DEFAULT_DIAL_DOT_SIZE * width : BWIDGETS_DEFAULT_DIAL_DOT_SIZE * height),
			 (width < height ? BWIDGETS_DEFAULT_DIAL_DOT_SIZE * width : BWIDGETS_DEFAULT_DIAL_DOT_SIZE * height),
			 name),
		focusLabel(0 ,0, 40, 20, name_ + BWIDGETS_DEFAULT_FOCUS_NAME, ""),
		fgColors (BWIDGETS_DEFAULT_FGCOLORS), bgColors (BWIDGETS_DEFAULT_BGCOLORS)
{

	setClickable (true);
	setDraggable (true);
	setScrollable (true);
	setFocusable (true);
	knob.setClickable (false);
	knob.setDraggable (false);
	knob.setScrollable (false);
	knob.setFocusable (false);
	dot.setClickable (false);
	dot.setDraggable (false);
	dot.setScrollable (false);
	dot.setFocusable (false);
	add (knob);
	add (dot);

	std::string valstr = BUtilities::to_string (getValue());
	focusLabel.setText (valstr);
	focusLabel.setStacking (STACKING_OVERSIZE);
	focusLabel.resize ();
	focusLabel.hide ();
	add (focusLabel);
}

Dial::Dial (const Dial& that) :
		RangeWidget (that),
		Focusable (that),
		dialCenter (that.dialCenter), dialRadius (that.dialRadius),
		knob (that.knob), dot (that.dot), focusLabel (that.focusLabel),
		fgColors (that.fgColors), bgColors (that.bgColors)
{
	add (knob);
	add (dot);
	focusLabel.hide();
	add (focusLabel);
}

Dial& Dial::operator= (const Dial& that)
{
	release (&knob);
	release (&dot);
	release (&focusLabel);

	knob = that.knob;
	dot = that.dot;
	focusLabel = that.focusLabel;
	focusLabel.hide();
	fgColors = that.fgColors;
	bgColors = that.bgColors;
	dialCenter = that.dialCenter;
	dialRadius = that.dialRadius;

	RangeWidget::operator= (that);
	Focusable::operator= (that);

	add (knob);
	add (dot);
	add (focusLabel);

	return *this;
}

Widget* Dial::clone () const {return new Dial (*this);}

void Dial::setValue (const double val)
{
	RangeWidget::setValue (val);
	std::string valstr = BUtilities::to_string (value);
	focusLabel.setText(valstr);
	focusLabel.resize ();
}

void Dial::update ()
{
	// Recalculate positions of the widget elements
	updateCoords ();

	// Draw scale
	draw (BUtilities::RectArea (0, 0, getWidth (), getHeight ()));

	// Update knob
	knob.moveTo (dialCenter.x - BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * dialRadius, dialCenter.y - BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * dialRadius);
	knob.resize (2 * BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * dialRadius, 2 * BWIDGETS_DEFAULT_DIAL_KNOB_SIZE * dialRadius);

	// Update dot
	double relVal = getRelativeValue ();
	dot.moveTo
	(
		dialCenter.x + 0.40 * dialRadius * cos (M_PI * (0.8 + 1.4 *relVal)) - BWIDGETS_DEFAULT_DIAL_DOT_SIZE * dialRadius,
		dialCenter.y + 0.40 * dialRadius * sin (M_PI * (0.8 + 1.4 *relVal)) - BWIDGETS_DEFAULT_DIAL_DOT_SIZE * dialRadius
	);
	dot.resize (2 * BWIDGETS_DEFAULT_DIAL_DOT_SIZE * dialRadius, 2 * BWIDGETS_DEFAULT_DIAL_DOT_SIZE * dialRadius);
	drawDot ();
	dot.update ();

	// Update focusLabel
	focusLabel.resize ();

	if (isVisible ()) postRedisplay ();
}

void Dial::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void Dial::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);
	knob.applyTheme (theme, name);
	focusLabel.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);

	// Foreground colors (active part arc, dot)
	void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
	if (fgPtr) fgColors = *((BColors::ColorSet*) fgPtr);

	// Background colors (passive part arc)
	void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
	if (bgPtr) bgColors = *((BColors::ColorSet*) bgPtr);

	if (fgPtr || bgPtr)
	{
		update ();
	}
}

void Dial::onButtonPressed (BEvents::PointerEvent* event)
{
	// Perform only if minimum requirements are satisfied
	if (main_ && isVisible () && (event->getButton () == BDevices::LEFT_BUTTON))
	{
		BUtilities::Point pos = event->getPosition ();
		double dist = (sqrt (pow (pos.x - dialCenter.x, 2) + pow (pos.y - dialCenter.y, 2)));
		double min = getMin ();
		double max = getMax ();

		// Direct dial point setting in hardChangeable mode
		if (hardChangeable)
		{
			if (dist >= 0.1 * dialRadius)
			{
				double angle = atan2 (pos.x - dialCenter.x, dialCenter.y - pos.y) + M_PI;
				if ((angle >= 0.2 * M_PI) && (angle <= 1.8 * M_PI))
				{
					double corrAngle = LIMIT (angle, 0.25 * M_PI, 1.75 * M_PI);
					double frac = (corrAngle - 0.25 * M_PI) / (1.5 * M_PI);
					if (getStep () < 0) frac = 1 - frac;
					setValue (getMin () + frac * (getMax () - getMin ()));

				}
			}
		}

		// Otherwise relative value change by dragging up or down
		// TODO Isn't there really no way to turn the dial in a sensitive but
		// safe way?
		else
		{
			if ((min != max) && (dialRadius >= 1))
			{
				double deltaFrac = -event->getDelta ().y / (dialRadius * 1.5 * M_PI);
				if (getStep () < 0) deltaFrac = -deltaFrac;
				softValue += deltaFrac * (max - min);
				setValue (getValue() + softValue);
			}
		}
	}
}

void Dial::onButtonReleased (BEvents::PointerEvent* event) {softValue = 0.0;}

void Dial::onPointerDragged (BEvents::PointerEvent* event) {onButtonPressed (event);}

void Dial::onWheelScrolled (BEvents::WheelEvent* event)
{
	double min = getMin ();
	double max = getMax ();

	if ((min != max) && (dialRadius >= 1))
	{
		double step = (getStep () != 0 ? getStep () : (max - min) / (dialRadius * 1.5 * M_PI));
		setValue (getValue() + event->getDelta ().y * step);
	}
}

void Dial::onFocusIn (BEvents::FocusEvent* event)
{
	if (event && event->getWidget())
	{
		BUtilities::Point pos = event->getPosition();
		focusLabel.moveTo (pos.x - 0.5 * focusLabel.getWidth(), pos.y - focusLabel.getHeight());
		focusLabel.show();
	}
	Widget::onFocusIn (event);
}
void Dial::onFocusOut (BEvents::FocusEvent* event)
{
	if (event && event->getWidget()) focusLabel.hide();
	Widget::onFocusOut (event);
}

void Dial::drawDot ()
{
	//cairo_surface_clear (dot.getDrawingSurface ());
	cairo_t* cr = cairo_create (dot.getDrawingSurface ());
	if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
	{
		double dotsize = dot.getWidth ();
		double dotrad = (dotsize > 2 ? dotsize / 2 - 1 : 0);
		BColors::Color fg = *fgColors.getColor (getState ()); fg.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
		cairo_pattern_t* pat = cairo_pattern_create_radial (dotsize / 2, dotsize / 2, 0.0, dotsize / 2, dotsize / 2, dotrad);
		cairo_pattern_add_color_stop_rgba (pat, 0, fg.getRed (), fg.getGreen (), fg.getBlue (), fg.getAlpha ());
		cairo_pattern_add_color_stop_rgba (pat, 1, fg.getRed (), fg.getGreen (), fg.getBlue (), 0.0);
		cairo_arc (cr, dotsize / 2, dotsize / 2, dotrad, 0, 2 * M_PI);
		cairo_close_path (cr);
		cairo_set_line_width (cr, 0.0);
		cairo_set_source (cr, pat);
		cairo_fill (cr);
		cairo_pattern_destroy (pat);
		cairo_destroy (cr);
	}
}

void Dial::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();
	dialRadius = (w < h ? w / 2 : h / 2);
	dialCenter.x = getWidth () / 2;
	dialCenter.y = getHeight () / 2;
}

void Dial::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	// Draw dial
	// only if minimum requirements satisfied
	if (dialRadius >= 12)
	{
		// Draw super class widget elements first
		Widget::draw (area);

		cairo_t* cr = cairo_create (widgetSurface_);

		if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
		{
			cairo_pattern_t* pat;

			// Limit cairo-drawing area
			cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
			cairo_clip (cr);

			double relVal = getRelativeValue ();

			// Colors uses within this method
			BColors::Color fgHi = *fgColors.getColor (getState ()); fgHi.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
			BColors::Color fgMid = *fgColors.getColor (getState ()); fgMid.applyBrightness ((BWIDGETS_DEFAULT_ILLUMINATED + BWIDGETS_DEFAULT_NORMALLIGHTED) / 2);
			BColors::Color fgLo = *fgColors.getColor (getState ()); fgLo.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color bgLo = *bgColors.getColor (getState ()); bgLo.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
			BColors::Color bgHi = *bgColors.getColor (getState ()); bgHi.applyBrightness (BWIDGETS_DEFAULT_ILLUMINATED);
			BColors::Color bgMid = *bgColors.getColor (getState ()); bgMid.applyBrightness ((BWIDGETS_DEFAULT_ILLUMINATED + BWIDGETS_DEFAULT_NORMALLIGHTED) / 2);
			BColors::Color bgSh = *bgColors.getColor (getState ()); bgSh.applyBrightness (BWIDGETS_DEFAULT_SHADOWED);

			// Arc
			cairo_set_source_rgba (cr, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
			cairo_set_line_width (cr, 0.0);
			cairo_arc (cr, dialCenter.x, dialCenter.y, 0.96 * dialRadius, M_PI * 0.75, M_PI * 2.25);
			cairo_arc_negative (cr, dialCenter.x, dialCenter.y ,  0.70 * dialRadius, M_PI * 2.25, M_PI * 0.75);
			cairo_close_path (cr);
			cairo_fill (cr);

			// Illumination arc top left
			pat = cairo_pattern_create_linear (dialCenter.x + dialRadius, dialCenter.y + dialRadius,
				dialCenter.x - dialRadius, dialCenter.y - dialRadius);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 1, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
				cairo_set_line_width (cr, 0.0);
				cairo_arc (cr, dialCenter.x, dialCenter.y, 0.96 * dialRadius, M_PI * 0.75, M_PI * 1.75);
				cairo_arc_negative (cr, dialCenter.x + BWIDGETS_DEFAULT_DIAL_DEPTH, dialCenter.y + BWIDGETS_DEFAULT_DIAL_DEPTH,  0.96 * dialRadius,
					M_PI * 1.75, M_PI * 0.75);
				cairo_close_path (cr);
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}

			// Illumination arc bottom right
			pat = cairo_pattern_create_linear (dialCenter.x + dialRadius, dialCenter.y + dialRadius,
				dialCenter.x - dialRadius, dialCenter.y - dialRadius);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
				cairo_arc_negative (cr, dialCenter.x, dialCenter.y, 0.70 * dialRadius, M_PI * 2.25, M_PI * 1.75);
				cairo_arc (cr, dialCenter.x + BWIDGETS_DEFAULT_DIAL_DEPTH, dialCenter.y + BWIDGETS_DEFAULT_DIAL_DEPTH,  0.70 * dialRadius,
					M_PI * 1.75, M_PI * 2.25);
				cairo_close_path (cr);
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}

			// Fill
			pat = cairo_pattern_create_linear (dialCenter.x + dialRadius, dialCenter.y + dialRadius,
   	   	   		dialCenter.x - dialRadius, dialCenter.y - dialRadius);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0.0, fgLo.getRed (), fgLo.getGreen (), fgLo.getBlue (), fgLo.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0.25, fgMid.getRed (), fgMid.getGreen (), fgMid.getBlue (), fgMid.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, fgLo.getRed (), fgLo.getGreen (), fgLo.getBlue (), fgLo.getAlpha ());
				if (getStep () >= 0)
				{
					cairo_arc (cr, dialCenter.x, dialCenter.y,  0.96 * dialRadius - 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH, M_PI * 0.75, M_PI * (0.75 + 1.5 * relVal));
					cairo_arc_negative (cr, dialCenter.x, dialCenter.y, 0.70 * dialRadius + 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH, M_PI * (0.75 + 1.5 * relVal), M_PI * 0.75);
				}
				else
				{
					cairo_arc (cr, dialCenter.x, dialCenter.y,  0.96 * dialRadius - 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH, M_PI * (0.75 + 1.5 * relVal), M_PI * 2.25);
					cairo_arc_negative (cr, dialCenter.x, dialCenter.y, 0.70 * dialRadius + 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH, M_PI * 2.25, M_PI * (0.75 + 1.5 * relVal));
				}
				cairo_close_path (cr);
				cairo_set_source (cr, pat);
				cairo_fill (cr);
				cairo_pattern_destroy (pat);
			}


			// Edges of the arc

			pat = cairo_pattern_create_linear (dialCenter.x + dialRadius, dialCenter.y + dialRadius,
	   	   	   	   	   	   	   	   	   	   	   dialCenter.x - dialRadius, dialCenter.y - dialRadius);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 0, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 1, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
				cairo_set_line_width (cr, 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH);

				cairo_arc_negative (cr, dialCenter.x, dialCenter.y,  0.70 * dialRadius, M_PI * 2.25, M_PI * 0.75);
				cairo_line_to (cr, dialCenter.x + 0.96 * dialRadius * cos (M_PI * 0.75), dialCenter.y + 0.96 * dialRadius * sin (M_PI * 0.75));
				cairo_set_source (cr, pat);
				cairo_stroke (cr);
				cairo_pattern_destroy (pat);
			}

			pat = cairo_pattern_create_linear (dialCenter.x + dialRadius, dialCenter.y + dialRadius,
 	   	   		dialCenter.x - dialRadius, dialCenter.y - dialRadius);
			if (pat && (cairo_pattern_status (pat) == CAIRO_STATUS_SUCCESS))
			{
				cairo_pattern_add_color_stop_rgba (pat, 1, bgHi.getRed (), bgHi.getGreen (), bgHi.getBlue (), bgHi.getAlpha ());
				cairo_pattern_add_color_stop_rgba (pat, 0, bgSh.getRed (), bgSh.getGreen (), bgSh.getBlue (), bgSh.getAlpha ());
				cairo_set_line_width (cr, 0.2 * BWIDGETS_DEFAULT_DIAL_DEPTH);

				cairo_arc (cr, dialCenter.x, dialCenter.y,  0.96 * dialRadius, M_PI * 0.75, M_PI * 2.25);
				cairo_line_to (cr, dialCenter.x + 0.70 * dialRadius * cos (M_PI * 2.25), dialCenter.y + 0.70 * dialRadius * sin (M_PI * 2.25));
				cairo_set_source (cr, pat);
				cairo_stroke (cr);
				cairo_pattern_destroy (pat);
			}

		}

		cairo_destroy (cr);
	}
}

}
