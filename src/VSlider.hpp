/* B.Harvestr
 * LV2 Plugin
 *
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

#ifndef VSLIDER_HPP_
#define VSLIDER_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "BUtilities/to_string.hpp"

class VSlider : public BWidgets::RangeWidget
{
public:
	VSlider () : VSlider (0, 0, 0, 0, "", 0, 0, 0, 0) {}
	VSlider (const double x, const double y, const double width, const double height, const std::string& name,
		 const double value, const double min, const double max, const double step, std::string format = "",
		 std::function<double (double x)> displayfunc = [] (double x) {return x;},
 	 	 std::function<double (double x)> func = [] (double x) {return x;},
 		 std::function<double (double x)> revfunc = [] (double x) {return x;}) :
		 RangeWidget (x, y, width, height, name, value, min, max, step),
		 txColors_ (BWIDGETS_DEFAULT_FGCOLORS),
		 fgColors_ (BWIDGETS_DEFAULT_FGCOLORS),
		 bgColors_ (BWIDGETS_DEFAULT_FGCOLORS),
		 format_ (format),
		 display_ (displayfunc),
		 transform_ (func),
		 reverse_ (revfunc)
	{
		setDraggable (true);
	}

	virtual Widget* clone () const override {return new VSlider (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);

		// Text colors
		void* txPtr = theme.getStyle(name, BWIDGETS_KEYWORD_TEXTCOLORS);
		if (txPtr) txColors_ = *((BColors::ColorSet*) txPtr);

		// Foreground colors (scale)
		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr) fgColors_ = *((BColors::ColorSet*) fgPtr);

		// Background colors (scale background, knob)
		void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
		if (bgPtr) bgColors_ = *((BColors::ColorSet*) bgPtr);

		if (fgPtr || bgPtr) update ();

	}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override {}

	virtual void onButtonReleased (BEvents::PointerEvent* event) override {}

	virtual void onPointerDragged (BEvents::PointerEvent* event) override
	{
		if (!event) return;

		BUtilities::Point pos = event->getOrigin();
		double y0 = getYOffset();
		double h = getEffectiveHeight();

		if ((h == 0) || (pos.y < y0) || (pos.y > y0 + h) || (getMin() == getMax())) return;

		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed - event->getDelta ().y / h, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

	virtual void onWheelScrolled (BEvents::WheelEvent* event) override
	{
		if (!event) return;

		BUtilities::Point pos = event->getPosition();
		double y0 = getYOffset();
		double h = getEffectiveHeight();

		if ((h == 0) || (pos.y < y0) || (pos.y > y0 + h) || (getMin() == getMax())) return;

		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed + event->getDelta ().y / h, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

protected:
	BColors::ColorSet txColors_;
	BColors::ColorSet fgColors_;
	BColors::ColorSet bgColors_;
	std::string format_;
	std::function<double(double)> display_;
	std::function<double(double)> transform_;
	std::function<double(double)> reverse_;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		// Draw super class widget elements first
		Widget::draw (area);

		// Draw scale only if it is not a null widget
		if ((getEffectiveHeight() >= 1) && (getEffectiveWidth() >= 1))
		{
			cairo_surface_clear (widgetSurface_);
			cairo_t* cr = cairo_create (widgetSurface_);

			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				const double x0 = getXOffset ();
				const double y0 = getYOffset ();
				const double h = getEffectiveHeight ();
				const double w = getEffectiveWidth ();
				const double y1 = y0 + (1 - transform_ ((value - getMin()) / (getMax() - getMin()))) * h;;

				//BColors::Color bgColor = *bgColors_.getColor (BColors::OFF);
				BColors::Color slColor = *fgColors_.getColor (getState ());
				//BColors::Color txColor = *txColors_.getColor (getState ());
				//BColors::Color frColor= *bgColors_.getColor (getState ());

				// Slider bar
				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (slColor));
				cairo_pattern_t* pat = cairo_pattern_create_linear (0, y0 + h, 0, y0);
				cairo_pattern_add_color_stop_rgba (pat, 0, slColor.getRed (), slColor.getGreen (), slColor.getBlue (), 0);
				cairo_pattern_add_color_stop_rgba (pat, 1, slColor.getRed (), slColor.getGreen (), slColor.getBlue (), 0.5 * slColor.getAlpha ());
				cairo_set_source (cr, pat);
				cairo_rectangle (cr, x0, y1, w, y0 + h - y1);
				cairo_fill_preserve (cr);

				cairo_set_line_width (cr, 2.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (slColor));
				cairo_stroke (cr);
			}
			cairo_destroy (cr);
		}
	}
};

#endif /* VSLIDER_HPP_ */
