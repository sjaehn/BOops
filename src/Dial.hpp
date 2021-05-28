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

#ifndef DIAL_HPP_
#define DIAL_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "BWidgets/Label.hpp"
#include "BWidgets/Text.hpp"
#include "BWidgets/Focusable.hpp"
#include "BUtilities/to_string.hpp"
#include "BUtilities/stof.hpp"

class Dial : public BWidgets::RangeWidget, public BWidgets::Focusable
{
public:
	Dial 	() : Dial (0, 0, 0, 0, "", 0, 0, 0, 0) {}
	Dial 	(const double x, const double y, const double width, const double height, const std::string& name,
		 const double value, const double min, const double max, const double step, std::string format = "",
		 std::string unit = "",
		 std::function<double (double x)> displayfunc = [] (double x) {return x;},
		 std::function<double (double x)> revdisplayfunc = [] (double x) {return x;},
 	 	 std::function<double (double x)> func = [] (double x) {return x;},
 		 std::function<double (double x)> revfunc = [] (double x) {return x;},
 		 std::function<std::string ()> focusText = [] () {return "";}) :
			RangeWidget (x, y, width, height, name, value, min, max, step),
			Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
				   std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
			valueLabel_ (0, 0, 0, 0, name, ""),
			unitLabel_ (0, 0, 0, 0, name, unit),
			focusLabel_ (0, 0, 40, 20, name_ + BWIDGETS_DEFAULT_FOCUS_NAME, ""),
			format_ (format),
			display_ (displayfunc),
			revdisplay_ (revdisplayfunc),
			transform_ (func),
			reverse_ (revfunc),
			focusText_ (focusText),
			txColors_ (BWIDGETS_DEFAULT_FGCOLORS),
			fgColors_ (BWIDGETS_DEFAULT_FGCOLORS),
			bgColors_ (BWIDGETS_DEFAULT_BGCOLORS)
	{
		setDraggable (true);
		setFocusable (true);
		valueLabel_.setEditable (true);
		valueLabel_.setScrollable (false);
		valueLabel_.setFocusable (false);
		unitLabel_.setScrollable (false);
		unitLabel_.setClickable (false);
		unitLabel_.setFocusable (false);
		valueLabel_.setCallbackFunction(BEvents::EventType::POINTER_DRAG_EVENT, displayDraggedCallback);
		valueLabel_.setCallbackFunction(BEvents::EventType::MESSAGE_EVENT, displayMessageCallback);
		add (valueLabel_);
		add (unitLabel_);

		focusLabel_.setStacking (BWidgets::STACKING_OVERSIZE);
		focusLabel_.setText (focusText_());
   		focusLabel_.hide ();
   		add (focusLabel_);
	}

	Dial (const Dial& that) :
		RangeWidget (that),
		Focusable (that),
		valueLabel_ (that.valueLabel_),
		unitLabel_ (that.unitLabel_),
		focusLabel_ (that.focusLabel_),
		format_ (that.format_),
		display_ (that.display_),
		revdisplay_ (that.revdisplay_),
		transform_ (that.transform_),
		reverse_ (that.reverse_),
		focusText_ (that.focusText_),
		txColors_ (that.txColors_),
		fgColors_ (that.fgColors_),
		bgColors_ (that.bgColors_)
	{
		add (valueLabel_);
		add (unitLabel_);
		add (focusLabel_);
	}

	Dial& operator= (const Dial& that)
	{
		release (&valueLabel_);
		release (&unitLabel_);
		release (&focusLabel_);
		valueLabel_ = that.valueLabel_;
		unitLabel_ = that.unitLabel_;
		focusLabel_ = that.focusLabel_;
		txColors_ = that.txColors_;
		fgColors_ = that.fgColors_;
		bgColors_ = that.bgColors_;
		format_ = that.format_;
		display_ = that.display_;
		revdisplay_ = that.revdisplay_;
		transform_ = that.transform_;
		reverse_ = that.reverse_;
		focusText_ = that.focusText_;
		RangeWidget::operator= (that);
		Focusable::operator= (that);
		add (valueLabel_);
		add (unitLabel_);
		add (focusLabel_);
		return *this;
	}

	virtual Widget* clone () const override {return new Dial (*this);}

	virtual void setUnit (const std::string& s)
	{
		unitLabel_.setText (s);
		update();
	}

	virtual void setFocusText (std::function<std::string()> fn)
	{
		focusText_ = fn;
		focusLabel_.setText (focusText_());
	}

	virtual void update () override
	{
		RangeWidget::update();

		focusLabel_.setText (focusText_());
   		focusLabel_.resize ();

		const double x0 = getXOffset ();
		const double y0 = getYOffset ();
		const double h = getEffectiveHeight ();
		const double w = getEffectiveWidth ();
		const std::string valstr = BUtilities::to_string (display_ (getValue()), format_);
		valueLabel_.setText (valstr);
		valueLabel_.resize();
		unitLabel_.resize();
		const double vw = valueLabel_.getWidth();
		const double vh = valueLabel_.getHeight();
		const double uw = unitLabel_.getWidth();
		const double uh = (unitLabel_.getText() != "" ? unitLabel_.getHeight() : 0);
		valueLabel_.moveTo (x0 + 0.5 * w - 0.5 * vw, y0 + 0.5 * h - 0.5 * (vh + uh));
		if (uh != 0) unitLabel_.moveTo (x0 + 0.5 * w - 0.5 * uw, y0 + 0.5 * h - 0.5 * (vh + uh) + vh);
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);
		valueLabel_.applyTheme (theme);
		unitLabel_.applyTheme (theme);
		focusLabel_.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);

		// Text colors
		void* txPtr = theme.getStyle(name, BWIDGETS_KEYWORD_TEXTCOLORS);
		if (txPtr) txColors_ = *((BColors::ColorSet*) txPtr);

		// Foreground colors (scale)
		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr) fgColors_ = *((BColors::ColorSet*) fgPtr);

		// Background colors (scale background, knob)
		void* bgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_BGCOLORS);
		if (bgPtr) bgColors_ = *((BColors::ColorSet*) bgPtr);

		if (txPtr || fgPtr || bgPtr) update ();

	}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override {}

	virtual void onButtonReleased (BEvents::PointerEvent* event) override {}

	virtual void onPointerDragged (BEvents::PointerEvent* event) override
	{
		if (!event) return;

		double w = getEffectiveWidth();
		double h = getEffectiveHeight();
		double d = (w < h ? w : h);

		if ((d == 0) || (getMin() == getMax())) return;

		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed - event->getDelta ().y / 2.0 / w, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

	virtual void onWheelScrolled (BEvents::WheelEvent* event) override
	{
		if (!event) return;

		double w = getEffectiveWidth();
		double h = getEffectiveHeight();
		double d = (w < h ? w : h);

		if ((d == 0) || (getMin() == getMax())) return;

		double dist = getMax() - getMin();
		double valueTransformed = transform_ ((getValue() - getMin()) / dist);
		double nval = LIMIT (valueTransformed + event->getDelta ().y / 2.0 / w, 0.0, 1.0);
		setValue (getMin() + reverse_ (nval) * dist);
	}

	virtual void onFocusIn (BEvents::FocusEvent* event) override
	{
		if (event && event->getWidget())
		{
			if (focusText_() != "")
			{
				BUtilities::Point pos = event->getPosition();
				//if (getParent()) getParent()->raiseToTop();
				//raiseToTop();

				// Resize
	                        cairo_surface_t* surface = widgetSurface_;
	                	cairo_t* cr = cairo_create (surface);
				focusLabel_.setText (focusText_());
	                        focusLabel_.resize (400,200);	// Maximize size first to omit breaks
	                	std::vector<std::string> textblock = focusLabel_.getTextBlock ();
	                	double blockheight = focusLabel_.getTextBlockHeight (textblock);
	                	double blockwidth = 0.0;
	                	for (std::string textline : textblock)
	                	{
	                		cairo_text_extents_t ext = focusLabel_.getFont ()->getTextExtents (cr, textline);
	                		if (ext.width > blockwidth) blockwidth = ext.width;
	                	}
				cairo_destroy (cr);
	                	focusLabel_.resize (blockwidth + 2 * focusLabel_.getXOffset (), blockheight + 2 * focusLabel_.getYOffset ());
	                	focusLabel_.resize();

				focusLabel_.moveTo (pos.x - 0.5 * focusLabel_.getWidth(), pos.y - focusLabel_.getHeight());
				focusLabel_.show();
			}

			else focusLabel_.hide();
		}

		Widget::onFocusIn (event);
	}

	virtual void onFocusOut (BEvents::FocusEvent* event) override
	{
		if (event && event->getWidget()) focusLabel_.hide();
		Widget::onFocusOut (event);
	}

protected:
	BWidgets::Label valueLabel_;
	BWidgets::Label unitLabel_;
	BWidgets::Text focusLabel_;
	std::string format_;
	std::function<double(double)> display_;
	std::function<double(double)> revdisplay_;
	std::function<double(double)> transform_;
	std::function<double(double)> reverse_;
	std::function<std::string()> focusText_;
	BColors::ColorSet txColors_;
	BColors::ColorSet fgColors_;
	BColors::ColorSet bgColors_;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		// Draw super class widget elements first
		Widget::draw (area);

		const double x0 = getXOffset ();
		const double y0 = getYOffset ();
		const double h = getEffectiveHeight ();
		const double w = getEffectiveWidth ();
		const double d = (w < h ? w : h);
		const double xc = x0 + 0.5 * w;
		const double yc = y0 + 0.5 * h;
		const double alpha = transform_ ((value - getMin()) / (getMax() - getMin()));

		// Draw scale only if it is not a null widget
		if (d > 0)
		{
			cairo_surface_clear (widgetSurface_);
			cairo_t* cr = cairo_create (widgetSurface_);

			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				BColors::Color slColor = *fgColors_.getColor (getState ()); slColor.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);
				BColors::Color bgColor = *bgColors_.getColor (getState ());
				BColors::Color txColor = *txColors_.getColor (getState ()); txColor.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);

				// Frame arc
				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (bgColor));
				cairo_arc (cr,xc, yc, 0.42 * d, M_PI * 0.52, M_PI * (0.52 + 1.96));
				cairo_arc_negative (cr, xc, yc , 0.32 * d, M_PI * (0.52 + 1.96), M_PI * 0.52);
				cairo_close_path (cr);
				cairo_fill (cr);

				// Arc
				cairo_set_line_width (cr, 0.0);
				cairo_set_source_rgba (cr, CAIRO_RGBA (slColor));
				cairo_arc (cr,xc, yc, 0.42 * d, M_PI * 0.52, M_PI * (0.52 + alpha * 1.96));
				cairo_arc_negative (cr, xc, yc , 0.32 * d, M_PI * (0.52 + alpha * 1.96), M_PI * 0.52);
				cairo_close_path (cr);
				cairo_fill (cr);
			}
			cairo_destroy (cr);
		}
	}

	static void displayDraggedCallback (BEvents::Event* event)
	{
		if (event && event->getWidget())
		{
			BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
			Dial* d = (Dial*)l->getParent();
			if (d && (!l->getEditMode())) d->Dial::onPointerDragged ((BEvents::PointerEvent*)event);
		}
	}

	static void displayMessageCallback (BEvents::Event* event)
	{
		if (event && event->getWidget())
		{
			BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
			Dial* d = (Dial*)l->getParent();
			if (d)
			{
				double val;
				try {val = BUtilities::stof (l->getText());}
				catch (std::invalid_argument &ia)
				{
					fprintf (stderr, "%s\n", ia.what());
					d->update();
					return;
				}

				d->setValue (d->revdisplay_ (val));
				d->update();
			}
		}
	}
};

#endif /* DIAL_HPP_ */
