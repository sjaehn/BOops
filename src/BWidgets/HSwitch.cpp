/* HSwitch.cpp
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

#include "HSwitch.hpp"

namespace BWidgets
{
HSwitch::HSwitch () : HSwitch (0.0, 0.0, BWIDGETS_DEFAULT_HSWITCH_WIDTH, BWIDGETS_DEFAULT_HSWITCH_HEIGHT, "hswitch", BWIDGETS_DEFAULT_VALUE) {}

HSwitch::HSwitch (const double  x, const double y, const double width, const double height, const std::string& name,
				  const double defaultvalue) :
		HSlider (x, y, width, height, name, defaultvalue, 0.0, 1.0, 1.0), dragged (false) {}

Widget* HSwitch::clone () const {return new HSwitch (*this);}

void HSwitch::onButtonPressed (BEvents::PointerEvent* event) {dragged = false;}

void HSwitch::onButtonReleased (BEvents::PointerEvent* event)
{
	if (!dragged)
	{
		if (getValue() == getMin ()) setValue (getMax ());
		else setValue (getMin ());
	}
}

void HSwitch::onPointerDragged (BEvents::PointerEvent* event)
{
	dragged = true;
	HScale::onButtonPressed (event);
}

void HSwitch::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();

	knobRadius = (h < w / 2 ? h / 2 : w / 4);
	scaleArea = BUtilities::RectArea
	(
		getXOffset (),
		getYOffset () + h / 2 - knobRadius,
		w,
		2 * knobRadius
	);
	scaleXValue = scaleArea.getX() + knobRadius + getRelativeValue () * (scaleArea.getWidth() - 2 * knobRadius);
	knobPosition = BUtilities::Point
	(
		scaleXValue + BWIDGETS_DEFAULT_KNOB_DEPTH,
		scaleArea.getY() + scaleArea.getHeight() / 2 + BWIDGETS_DEFAULT_KNOB_DEPTH
	);
}

}
