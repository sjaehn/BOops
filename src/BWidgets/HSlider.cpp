/* HSlider.cpp
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

#include "HSlider.hpp"
#include "../BUtilities/to_string.hpp"

namespace BWidgets
{
HSlider::HSlider () : HSlider (0.0, 0.0, BWIDGETS_DEFAULT_HSLIDER_WIDTH, BWIDGETS_DEFAULT_HSLIDER_HEIGHT, "hslider",
		  	  	  	  	  	   BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP) {}

HSlider::HSlider (const double  x, const double y, const double width, const double height, const std::string& name,
				  const double value, const double min, const double max, const double step) :
		HScale (x, y, width, height, name, value, min, max, step),
		Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
			std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
		knob (0, 0, 0, 0, BWIDGETS_DEFAULT_KNOB_DEPTH, name),
		focusLabel(0 ,0, 40, 20, name_ + BWIDGETS_DEFAULT_FOCUS_NAME, ""),
		knobRadius (0), knobPosition ()
{
	setFocusable (true);

	knob.setClickable (false);
	knob.setDraggable (false);
	knob.setScrollable (false);
	knob.setFocusable (false);
	add (knob);

	std::string valstr = BUtilities::to_string (getValue());
	focusLabel.setText (valstr);
	focusLabel.setStacking (STACKING_OVERSIZE);
	focusLabel.resize ();
	focusLabel.hide ();
	add (focusLabel);
}

HSlider::HSlider (const HSlider& that) :
		HScale (that), Focusable (that),
		knob (that.knob), focusLabel (that.focusLabel),
		knobRadius (that.knobRadius), knobPosition (that.knobPosition)
{
	add (knob);
	focusLabel.hide();
	add (focusLabel);
}

HSlider& HSlider::operator= (const HSlider& that)
{
	release (&knob);
	release (&focusLabel);

	knob = that.knob;
	focusLabel = that.focusLabel;
	focusLabel.hide();
	knobRadius = that.knobRadius;
	knobPosition = that.knobPosition;
	HScale::operator= (that);
	Focusable::operator= (that);

	add (knob);
	add (focusLabel);

	return *this;
}

Widget* HSlider::clone () const {return new HSlider (*this);}

void HSlider::setValue (const double val)
{
	RangeWidget::setValue (val);
	std::string valstr = BUtilities::to_string (value);
	focusLabel.setText(valstr);
	focusLabel.resize ();
}

void HSlider::update ()
{
	HScale::update ();

	// Update Knob
	knob.moveTo (knobPosition.x - knobRadius, knobPosition.y - knobRadius);
	knob.setWidth (2 * knobRadius);
	knob.setHeight (2 * knobRadius);

	// Update focusLabel
	focusLabel.resize ();
}

void HSlider::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void HSlider::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	HScale::applyTheme (theme, name);
	knob.applyTheme (theme, name);
	focusLabel.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);
}

void HSlider::onFocusIn (BEvents::FocusEvent* event)
{
	if (event && event->getWidget())
	{
		BUtilities::Point pos = event->getPosition();
		focusLabel.moveTo (pos.x - 0.5 * focusLabel.getWidth(), pos.y - focusLabel.getHeight());
		focusLabel.show();
	}
	Widget::onFocusIn (event);
}
void HSlider::onFocusOut (BEvents::FocusEvent* event)
{
	if (event && event->getWidget()) focusLabel.hide();
	Widget::onFocusOut (event);
}

void HSlider::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();

	knobRadius = (h < w / 2 ? h / 2 : w / 4);
	scaleArea = BUtilities::RectArea
	(
		getXOffset () + knobRadius,
		getYOffset () + h / 2 - knobRadius / 2,
		w - 2 * knobRadius,
		knobRadius
	);
	scaleXValue = scaleArea.getX() + getRelativeValue () * scaleArea.getWidth();
	knobPosition = BUtilities::Point (scaleXValue, scaleArea.getY() + scaleArea.getHeight() / 2);
}
}
