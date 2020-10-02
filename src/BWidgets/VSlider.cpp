/* VSlider.cpp
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

#include "VSlider.hpp"
#include "../BUtilities/to_string.hpp"

namespace BWidgets
{
VSlider::VSlider () : VSlider (0.0, 0.0, BWIDGETS_DEFAULT_VSLIDER_WIDTH, BWIDGETS_DEFAULT_VSLIDER_HEIGHT, "vslider",
		  	  	  	  	  	   BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP) {}

VSlider::VSlider (const double  x, const double y, const double width, const double height, const std::string& name,
				  const double value, const double min, const double max, const double step) :
		VScale (x, y, width, height, name, value, min, max, step),
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

VSlider::VSlider (const VSlider& that) :
		VScale (that), Focusable (that),
		knob (that.knob), focusLabel (that.focusLabel),
		knobRadius (that.knobRadius), knobPosition (that.knobPosition)
{
	add (knob);
	focusLabel.hide();
	add (focusLabel);
}

VSlider& VSlider::operator= (const VSlider& that)
{
	release (&knob);
	release (&focusLabel);

	knob = that.knob;
	focusLabel = that.focusLabel;
	focusLabel.hide();
	knobRadius = that.knobRadius;
	knobPosition = that.knobPosition;
	RangeWidget::operator= (that);
	Focusable::operator= (that);

	add (knob);
	add (focusLabel);

	return *this;
}

Widget* VSlider::clone () const {return new VSlider (*this);}

void VSlider::setValue (const double val)
{
	RangeWidget::setValue (val);
	std::string valstr = BUtilities::to_string (value);
	focusLabel.setText(valstr);
	focusLabel.resize ();
}

void VSlider::update ()
{
	VScale::update ();

	// Update Knob
	knob.moveTo (knobPosition.x -  knobRadius, knobPosition.y - knobRadius);
	knob.resize (2 * knobRadius, 2 * knobRadius);

	// Update focusLabel
	focusLabel.resize ();
}

void VSlider::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void VSlider::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	VScale::applyTheme (theme, name);
	knob.applyTheme (theme, name);
	focusLabel.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);
}

void VSlider::onFocusIn (BEvents::FocusEvent* event)
{
	if (event && event->getWidget())
	{
		BUtilities::Point pos = event->getPosition();
		focusLabel.moveTo (pos.x - 0.5 * focusLabel.getWidth(), pos.y - focusLabel.getHeight());
		focusLabel.show();
	}
	Widget::onFocusIn (event);
}
void VSlider::onFocusOut (BEvents::FocusEvent* event)
{
	if (event && event->getWidget()) focusLabel.hide();
	Widget::onFocusOut (event);
}

void VSlider::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();

	knobRadius = (w < h ? w / 2 : h / 2);
	scaleArea = BUtilities::RectArea
	(
		getXOffset () + w / 2 - knobRadius / 2,
		getYOffset () + knobRadius,
		knobRadius,
		h - 2 * knobRadius
	);
	scaleYValue = scaleArea.getY() + (1 - getRelativeValue ()) * scaleArea.getHeight();
	knobPosition = BUtilities::Point
	(
		scaleArea.getX() + scaleArea.getWidth() / 2,
		scaleYValue
	);
}
}
