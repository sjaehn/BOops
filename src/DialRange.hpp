/* B.Noname01
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

#ifndef DIALRANGE_HPP_
#define DIALRANGE_HPP_

#include "Dial.hpp"
#include "BUtilities/to_string.hpp"

#ifndef RANGEDIRECTIONS
#define RANGEDIRECTIONS
enum RangeDirections
{
	UNIDIRECTIONAL	= 0,
	BIDIRECTIONAL	= 1
};
#endif /* RANGEDIRECTIONS */

class DialRange : public Dial
{
public:
	DialRange () : DialRange (0, 0, 0, 0, "", 0, 0, 0, 0) {}
	DialRange (const double x, const double y, const double width, const double height, const std::string& name,
		 const double value, const double min, const double max, const double step,
		 RangeDirections dir = BIDIRECTIONAL,
		 std::string format = "",
		 std::string unit = "",
		 std::function<double (double x)> displayfunc = [] (double x) {return x;},
 	 	 std::function<double (double x)> func = [] (double x) {return x;},
 		 std::function<double (double x)> revfunc = [] (double x) {return x;}) :
			Dial (x, y, width, height, name, value, min, max, step, format, unit, displayfunc, func, revfunc),
			range (0, 0, 0, 0, name + "/range", 0.0, (dir == BIDIRECTIONAL ? 0.0 : min - max), max - min, step),
			direction (dir)
	{
		add (range);
	}

	DialRange (const DialRange& that) :
		Dial (that), range (that.range), direction (that.direction)
	{
		add (range);
	}

	DialRange& operator= (const DialRange& that)
	{
		release (&range);
		direction = that.direction;
		Dial::operator= (that);
		range = that.range;
		add (range);

		return *this;
	}

	virtual Widget* clone () const override {return new DialRange (*this);}

	virtual void setMin (const double min) override
	{
		Dial::setMin (min);
		range.setMin (min - getMax());
	}

	virtual void setMax (const double max) override
	{
		Dial::setMin (max);
		range.setMin (max - getMin());
	}

	virtual void setStep (const double step) override
	{
		Dial::setMin (step);
		range.setMin (step);
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Dial::applyTheme (theme, name);
		range.applyTheme (theme, name);
	}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override {}

	virtual void onButtonReleased (BEvents::PointerEvent* event) override {}

	virtual void onPointerDragged (BEvents::PointerEvent* event) override
	{
		if (main_ && isVisible () && event && (event->getButton () == BDevices::LEFT_BUTTON))

		{
			const double x0 = getXOffset ();
			const double y0 = getYOffset ();
			const double h = getEffectiveHeight ();
			const double w = getEffectiveWidth ();
			const double d = (w < h ? w : h);
			const double xc = x0 + 0.5 * w;
			const double yc = y0 + 0.5 * h;

			if ((d == 0) || (getMin() == getMax())) return;

			const BUtilities::Point ori = event->getOrigin ();
			const double oriDist = (sqrt (pow (ori.x - xc, 2) + pow (ori.y - yc, 2)));
			const double dist = getMax() - getMin();

			if (oriDist < 0.42 * d)
			{
				const double valueTransformed = transform_ ((getValue() - getMin()) / dist);
				const double nval = LIMIT (valueTransformed - event->getDelta ().y / 2.0 / w, 0.0, 1.0);
				setValue (getMin() + reverse_ (nval) * dist);
			}
			else
			{
				const double valueTransformed = transform_ ((range.getValue() - range.getMin()) / dist);
				const double nval = LIMIT (valueTransformed - event->getDelta ().y / 2.0 / w, 0.0, 1.0);
				range.setValue (range.getMin() + reverse_ (nval) * dist);
			}
		}
	}

	virtual void onWheelScrolled (BEvents::WheelEvent* event) override
	{
		if (main_ && isVisible () && event)

		{
			const double x0 = getXOffset ();
			const double y0 = getYOffset ();
			const double h = getEffectiveHeight ();
			const double w = getEffectiveWidth ();
			const double d = (w < h ? w : h);
			const double xc = x0 + 0.5 * w;
			const double yc = y0 + 0.5 * h;

			if ((d == 0) || (getMin() == getMax())) return;

			const BUtilities::Point ori = event->getPosition ();
			const double oriDist = (sqrt (pow (ori.x - xc, 2) + pow (ori.y - yc, 2)));
			const double dist = getMax() - getMin();

			if (oriDist < 0.42 * d)
			{
				const double valueTransformed = transform_ ((getValue() - getMin()) / dist);
				const double nval = LIMIT (valueTransformed - event->getDelta ().y / 2.0 / w, 0.0, 1.0);
				setValue (getMin() + reverse_ (nval) * dist);
			}
			else
			{
				const double valueTransformed = transform_ ((range.getValue() - range.getMin()) / dist);
				const double nval = LIMIT (valueTransformed - event->getDelta ().y / 2.0 / w, 0.0, 1.0);
				range.setValue (range.getMin() + reverse_ (nval) * dist);
			}
		}
	}

	BWidgets::RangeWidget range;

protected:
	RangeDirections direction;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		// Draw super class widget elements first
		Dial::draw (area);

		const double x0 = getXOffset ();
		const double y0 = getYOffset ();
		const double h = getEffectiveHeight ();
		const double w = getEffectiveWidth ();
		const double d = (w < h ? w : h);
		const double xc = x0 + 0.5 * w;
		const double yc = y0 + 0.5 * h;

		// Draw scale only if it is not a null widget
		if (d > 0)
		{
			cairo_t* cr = cairo_create (widgetSurface_);

			if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
			{
				// Limit cairo-drawing area
				cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
				cairo_clip (cr);

				BColors::Color txColor = *txColors_.getColor (getState ()); txColor.applyBrightness (BWIDGETS_DEFAULT_NORMALLIGHTED);

				cairo_new_path (cr);

				const double v0 = getRelativeValue ();
				const double vr = (getMax() != getMin() ? range.getValue() / (getMax() - getMin()) : 0.0);
				double v1 = transform_ (direction == UNIDIRECTIONAL ? v0 : LIMIT (v0 - vr, 0.0, 1.0));
				double v2 = transform_ (LIMIT (v0 + vr, 0.0, 1.0));
				if (v2 < v1) std::swap (v1, v2);
				const double p1 = M_PI * (0.52 + 1.96 * v1);
				const double p2 = M_PI * (0.52 + 1.96 * v2);

				// Arc
				cairo_set_source_rgba (cr, CAIRO_RGBA (txColor));
				cairo_set_line_width (cr, 0.0);
				cairo_arc (cr, xc, yc, 0.46 * d, p1, p2);
				cairo_arc_negative (cr, xc, yc ,  0.48 * d, p2, p1);
				cairo_close_path (cr);
				cairo_fill (cr);

				// Arrow
				cairo_save (cr);
				if ((direction == BIDIRECTIONAL) || (vr <= 0))
				{
					cairo_translate (cr, xc, yc);
					cairo_rotate (cr, p1);
					cairo_move_to (cr, 0.5 * d, 0);
					cairo_rel_line_to (cr, -0.06 * d, 0);
					cairo_rel_line_to (cr, 0.03 * d, -0.04 * d);
					cairo_close_path (cr);
					cairo_fill (cr);
				}
				if ((direction == BIDIRECTIONAL) || (vr > 0))
				{
					cairo_restore (cr);
					cairo_translate (cr, xc, yc);
					cairo_rotate (cr, p2);
					cairo_move_to (cr, 0.5 * d, 0);
					cairo_rel_line_to (cr, -0.06 * d, 0);
					cairo_rel_line_to (cr, 0.03 * d, 0.04 * d);
					cairo_close_path (cr);
					cairo_fill (cr);
				}

			}
			cairo_destroy (cr);
		}
	}
};

#endif /* DIALRANGE_HPP_ */
