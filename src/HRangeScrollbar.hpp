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

#ifndef HRANGESCROLLBAR_HPP_
#define HRANGESCROLLBAR_HPP_

#include "BWidgets/RangeWidget.hpp"

class HRangeScrollbar : public BWidgets::Widget
{
public:
	class EndButton : public BWidgets::RangeWidget
	{
	public:
		EndButton() : EndButton (0, 0, 0, 0, "button", 0, 0, 1, 0) {}

		EndButton (const double x, const double y, const double width, const double height, const std::string& name) :
			EndButton (x, y, width, height, name, 0, 0, 1, 0) {}

		EndButton (const double x, const double y, const double width, const double height, const std::string& name,
			   const double value, const double min, const double max, const double step) :
			RangeWidget (0, 0, width, height, name, value, min, max, step),
			bgColors (BWIDGETS_DEFAULT_BGCOLORS)
		{
			draggable_ = true;
		}

		virtual void setValue (const double val) override
		{
			if (val != getValue())
			{
				RangeWidget::setValue (val);
				if (getParent()) getParent()->update();
			}
		}

		virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

   		virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
		{
			RangeWidget::applyTheme (theme, name);

			void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
			if (fgPtr) fgColors = *((BColors::ColorSet*) fgPtr);

			void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
			if (bgPtr) bgColors = *((BColors::ColorSet*) bgPtr);

			if (bgPtr || fgPtr) update();
		}

		virtual void onPointerDragged (BEvents::PointerEvent* event) override
		{
			if ((!event) || (!getParent()) || (getParent()->getEffectiveWidth() - getWidth() <= 0)) return;
			double dx = event->getDelta().x;
			if (!dx) return;
			setValue (getValue() + dx * (getMax() - getMin()) / (getParent()->getEffectiveWidth() - getWidth()));
		}

	protected:
		BColors::ColorSet fgColors;
		BColors::ColorSet bgColors;

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

					BColors::Color fgColor = *fgColors.getColor (getState ());
					BColors::Color bgColor = *bgColors.getColor (getState ());

					cairo_arc (cr, x0 + 0.5 * w, y0 + 0.5 * h, (w > h ? 0.4 * h : 0.4 * w), 0.0, 2.0 * M_PI);
					cairo_set_line_width (cr, 0.0);
					cairo_set_source_rgba (cr, CAIRO_RGBA (fgColor));
					cairo_fill_preserve (cr);
					cairo_set_line_width (cr, (w > h ? 0.2 * h : 0.2 * w));
					cairo_set_source_rgba (cr, CAIRO_RGBA (bgColor));
					cairo_stroke (cr);
				}

				cairo_destroy (cr);
			}
		}
	};

	HRangeScrollbar() : HRangeScrollbar (0, 0, 0, 0, "scrollbar", 0, 0, 0, 1, 0) {}

	HRangeScrollbar (const double x, const double y, const double width, const double height, const std::string& name) :
		HRangeScrollbar (x, y, width, height, name, 0, 0, 0, 1, 0) {}

	HRangeScrollbar (const double x, const double y, const double width, const double height, const std::string& name,
			 const double minValue, const double maxValue, const double rangeMin, const double rangeMax, const double rangeStep) :
		Widget (x, y, width, height, name),
		minButton (0, 0, height, height, name, minValue, rangeMin, rangeMax, rangeStep),
		maxButton (0, 0, height, height, name, maxValue, rangeMin, rangeMax, rangeStep),
		fgColors (BWIDGETS_DEFAULT_FGCOLORS)
	{
		draggable_ = true;
		scrollable_ = true;
		add (minButton);
		add (maxButton);
	}

	HRangeScrollbar (const HRangeScrollbar& that) :
		Widget (that),
		minButton (that.minButton),
		maxButton (that.maxButton),
		fgColors (that.fgColors)
	{
		add (minButton);
		add (maxButton);
	}

	HRangeScrollbar& operator= (const HRangeScrollbar& that)
	{	release (&minButton);
		release (&maxButton);

		minButton = that.minButton;
		maxButton = that.maxButton;
		fgColors = that.fgColors;
		Widget::operator= (that);

		add (minButton);
		add (maxButton);

		return *this;
	}


	virtual void update() override
	{
		if (maxButton.getValue() < minButton.getValue())
		{
			maxButton.setValue (minButton.getValue());
			return;
		}

		Widget::update();
		minButton.resize (getHeight(), getHeight());
		maxButton.resize (getHeight(), getHeight());

		if ((getEffectiveWidth() - minButton.getWidth() > 0) && (minButton.getMin() != minButton.getMax()))
		{
			minButton.moveTo (minButton.getValue() * (getEffectiveWidth() - minButton.getWidth()) / (minButton.getMin() != minButton.getMax()), 0);
		}
		else minButton.moveTo (0, 0);

		if ((getEffectiveWidth() - maxButton.getWidth() > 0) && (maxButton.getMin() != maxButton.getMax()))
		{
			maxButton.moveTo (maxButton.getValue() * (getEffectiveWidth() - maxButton.getWidth()) / (maxButton.getMin() != maxButton.getMax()), 0);
		}
		else maxButton.moveTo (0, 0);
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
		if (fgPtr)
		{
			fgColors = *((BColors::ColorSet*) fgPtr);
			update();
		}
	}

	virtual void onPointerDragged (BEvents::PointerEvent* event) override
	{
		if ((!event) || (getEffectiveWidth() < 1.0)) return;

		double df = event->getDelta().x / getEffectiveWidth();
		if (minButton.getValue() + df < minButton.getMin()) df = minButton.getMin() - minButton.getValue();
		else if (maxButton.getValue() + df > maxButton.getMax()) df = maxButton.getMax() - maxButton.getValue();

		minButton.setValue (minButton.getValue() + df);
		maxButton.setValue (maxButton.getValue() + df);
	}

	virtual void onWheelScrolled (BEvents::WheelEvent* event) override
	{
		if ((!event) || (getEffectiveWidth() < 1.0)) return;

		const double c = 0.5 * (minButton.getValue() + maxButton.getValue());
		const double r = maxButton.getValue() - c;
		const double f = 1.0 + 0.1 * event->getDelta().y;

		minButton.setValue (c - r * f);
		maxButton.setValue (c + r * f);
	}

	EndButton minButton;
	EndButton maxButton;

protected:
	BColors::ColorSet fgColors;

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
				const double w0 = getEffectiveWidth ();
				const double w = (w0 > h ? w0 - h : 0);

				BColors::Color fgColor = *fgColors.getColor (getState ());

				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (fgColor));
				cairo_rectangle (cr, x0 + 0.5 * h + minButton.getRelativeValue() * w, y0, (maxButton.getRelativeValue() - minButton.getRelativeValue()) * w, h);
				cairo_fill (cr);
			}

			cairo_destroy (cr);
		}
	}



};

#endif /* HRANGESCROLLBAR_HPP_ */
