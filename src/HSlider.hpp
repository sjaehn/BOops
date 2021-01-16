/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef HSLIDER_HPP_
#define HSLIDER_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "BWidgets/Label.hpp"
#include "BUtilities/to_string.hpp"
#include "BUtilities/stof.hpp"

class HSlider : public BWidgets::RangeWidget
{
public:
	HSlider () : HSlider (0, 0, 0, 0, "", 0, 0, 0, 0) {}
	HSlider (const double x, const double y, const double width, const double height, const std::string& name,
		 const double value, const double min, const double max, const double step, std::string format = "",
		 std::function<double (double x)> displayfunc = [] (double x) {return x;},
		 std::function<double (double x)> revdisplayfunc = [] (double x) {return x;},
 	 	 std::function<double (double x)> func = [] (double x) {return x;},
 		 std::function<double (double x)> revfunc = [] (double x) {return x;}) :
			RangeWidget (x, y, width, height, name, value, min, max, step),
			valueLabel_ (0, 0, 0, 0, name, ""),
			txColors_ (BWIDGETS_DEFAULT_FGCOLORS),
			fgColors_ (BWIDGETS_DEFAULT_FGCOLORS),
			bgColors_ (BWIDGETS_DEFAULT_FGCOLORS),
			format_ (format),
			display_ (displayfunc),
			revdisplay_ (revdisplayfunc),
			transform_ (func),
			reverse_ (revfunc)
	{
		setDraggable (true);
		valueLabel_.setEditable (true);
		valueLabel_.setScrollable (false);
		valueLabel_.setCallbackFunction(BEvents::EventType::POINTER_DRAG_EVENT, displayDraggedCallback);
		valueLabel_.setCallbackFunction(BEvents::EventType::MESSAGE_EVENT, displayMessageCallback);
		add (valueLabel_);
	}

	HSlider (const HSlider& that) :
		RangeWidget (that),
		valueLabel_ (that.valueLabel_),
		txColors_ (that.txColors_),
		fgColors_ (that.fgColors_),
		bgColors_ (that.bgColors_),
		format_ (that.format_),
		display_ (that.display_),
		revdisplay_ (that.revdisplay_),
		transform_ (that.transform_),
		reverse_ (that.reverse_)
	{
		add (valueLabel_);
	}

	HSlider& operator= (const HSlider& that)
	{
		release (&valueLabel_);
		valueLabel_ = that.valueLabel_;
		txColors_ = that.txColors_;
		fgColors_ = that.fgColors_;
		bgColors_ = that.bgColors_;
		format_ = that.format_;
		display_ = that.display_;
		revdisplay_ = that.revdisplay_;
		transform_ = that.transform_;
		reverse_ = that.reverse_;
		RangeWidget::operator= (that);
		add (valueLabel_);
		return *this;
	}


	virtual Widget* clone () const override {return new HSlider (*this);}

	virtual void update () override
	{
		RangeWidget::update();

		const double x0 = getXOffset ();
		const double y0 = getYOffset ();
		const double h = getEffectiveHeight ();
		const double w = getEffectiveWidth ();
		const std::string valstr = BUtilities::to_string (display_ (getValue()), format_);
		valueLabel_.setText (valstr);
		valueLabel_.resize();
		const double vw = valueLabel_.getWidth();
		const double vh = valueLabel_.getHeight();
		valueLabel_.moveTo (x0 + 0.5 * w - 0.5 * vw, y0 + 0.5 * h - 0.5 * vh);
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);
		valueLabel_.applyTheme (theme);

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
		double x0 = getXOffset();
		double w = getEffectiveWidth();

		if ((w == 0) || (pos.x < x0) || (pos.x > x0 + w) || (getMin() == getMax())) return;

		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed + event->getDelta ().x / w, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

	virtual void onWheelScrolled (BEvents::WheelEvent* event) override
	{
		if (!event) return;

		BUtilities::Point pos = event->getPosition();
		double x0 = getXOffset();
		double w = getEffectiveWidth();

		if ((w == 0) || (pos.x < x0) || (pos.x > x0 + w) || (getMin() == getMax())) return;

		double step = (getStep() ? getStep() / (getMax() - getMin()) : 1.0 / w);
		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed + event->getDelta ().y * step, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

protected:
	BWidgets::Label valueLabel_;
	BColors::ColorSet txColors_;
	BColors::ColorSet fgColors_;
	BColors::ColorSet bgColors_;
	std::string format_;
	std::function<double(double)> display_;
	std::function<double(double)> revdisplay_;
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
				const double x1 = x0 + transform_ ((value - getMin()) / (getMax() - getMin())) * w;;

				BColors::Color bgColor = *bgColors_.getColor (BColors::OFF);
				BColors::Color slColor = *fgColors_.getColor (getState ());
				BColors::Color frColor= *bgColors_.getColor (getState ());

				// Slider bar
				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (bgColor));
				cairo_rectangle (cr, x0, y0, w, h);
				cairo_fill (cr);

				cairo_set_source_rgba (cr, CAIRO_RGBA (slColor));
				cairo_rectangle (cr, x0, y0, x1 - x0, h);
				cairo_fill (cr);

				cairo_set_line_width (cr, 1.0);
				cairo_rectangle (cr, x0, y0, w, h);
				cairo_set_source_rgba (cr, CAIRO_RGBA (frColor));
				cairo_stroke (cr);
			}
			cairo_destroy (cr);
		}
	}

	static void displayDraggedCallback (BEvents::Event* event)
	{
		if (event && event->getWidget())
		{
			BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
			HSlider* h = (HSlider*)l->getParent();
			if (h && (!l->getEditMode())) h->HSlider::onPointerDragged ((BEvents::PointerEvent*)event);
		}
	}

	static void displayMessageCallback (BEvents::Event* event)
	{
		if (event && event->getWidget())
		{
			BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
			HSlider* h = (HSlider*)l->getParent();
			if (h)
			{
				double val;
				try {val = BUtilities::stof (l->getText());}
				catch (std::invalid_argument &ia)
				{
					fprintf (stderr, "%s\n", ia.what());
					h->update();
					return;
				}

				h->setValue (h->revdisplay_ (val));
			}
		}
	}
};

#endif /* HSLIDER_HPP_ */
