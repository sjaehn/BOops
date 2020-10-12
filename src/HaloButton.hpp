/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
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

#ifndef HALOBUTTON_HPP_
#define HALOBUTTON_HPP_

#include "BWidgets/ValueWidget.hpp"
#include "BWidgets/Focusable.hpp"
#include "BWidgets/Label.hpp"

class HaloButton : public BWidgets::ValueWidget, public BWidgets::Focusable
{
public:
	HaloButton () : HaloButton (0, 0, 0, 0, "editwidget", "") {}

	HaloButton (const double x, const double y, const double width, const double height, const std::string& name, const std::string& focusText) :
		ValueWidget (x, y, width, height, name, 0.0),
		Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
			   std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
			   focusLabel_ (0, 0, 40, 20, name_ + BWIDGETS_DEFAULT_FOCUS_NAME, focusText)
   	{
   		focusLabel_.setStacking (BWidgets::STACKING_OVERSIZE);
   		focusLabel_.resize ();
   		focusLabel_.hide ();
   		add (focusLabel_);
   	}

	HaloButton (const HaloButton& that) :
		ValueWidget (that), Focusable (that),
		focusLabel_ (that.focusLabel_)
	{
		focusLabel_.hide();
		add (focusLabel_);
	}

	HaloButton& operator= (const HaloButton& that)
	{
		release (&focusLabel_);
		focusLabel_ = that.focusLabel_;
		focusLabel_.hide();

		Widget::operator= (that);
		Focusable::operator= (that);

		add (focusLabel_);

		return *this;

	}

	virtual BWidgets::Widget* clone () const override {return new HaloButton (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Widget::applyTheme (theme, name);
		focusLabel_.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);
		focusLabel_.resize();
	}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override
	{
		setValue (1.0);
		Widget::cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
	}

	virtual void onButtonReleased (BEvents::PointerEvent* event) override
	{
		setValue (0.0);
		Widget::cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);
	}

	virtual void onFocusIn (BEvents::FocusEvent* event) override
	{
		if (event && event->getWidget())
		{
			BUtilities::Point pos = event->getPosition();
			focusLabel_.moveTo (pos.x - 0.5 * focusLabel_.getWidth(), pos.y - focusLabel_.getHeight());
			focusLabel_.show();
		}
		Widget::onFocusIn (event);
	}

	virtual void onFocusOut (BEvents::FocusEvent* event) override
	{
		if (event && event->getWidget()) focusLabel_.hide();
		Widget::onFocusOut (event);
	}

protected:
	BWidgets::Label focusLabel_;

	void draw (const BUtilities::RectArea& area)
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 1) && (getHeight () >= 1))
		{
			// Draw super class widget elements first
			Widget::draw (area);

			if (value == 1.0)
			{
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
					cairo_rectangle (cr, x0, y0, w, h);
					cairo_set_line_width (cr, 1.0);
					cairo_set_source_rgba (cr, CAIRO_RGBA (BColors::white));
					cairo_stroke (cr);
					cairo_destroy (cr);
				}
			}
		}
	}
};

#endif /* HALOBUTTON_HPP_ */
