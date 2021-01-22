/* VSliderValue.cpp
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

#include "VSliderValue.hpp"
#include "../BUtilities/to_string.hpp"
#include "../BUtilities/stof.hpp"

namespace BWidgets
{
VSliderValue::VSliderValue () :
	VSliderValue
	(
		0.0,
		0.0,
		BWIDGETS_DEFAULT_VSLIDERVALUE_WIDTH,
		BWIDGETS_DEFAULT_VSLIDERVALUE_HEIGHT,
		"vslidervalue",
		BWIDGETS_DEFAULT_VALUE,
		BWIDGETS_DEFAULT_RANGE_MIN,
		BWIDGETS_DEFAULT_RANGE_MAX,
		BWIDGETS_DEFAULT_RANGE_STEP,
		BWIDGETS_DEFAULT_VALUE_FORMAT
	)
{}

VSliderValue::VSliderValue (const double x, const double y, const double width, const double height, const std::string& name,
	  		    const double value, const double min, const double max, const double step,
	  		    const std::string& valueFormat, LabelPosition valuePos) :
	VSlider (x, y, width, height, name, value, min, max, step),
	valueDisplay(0, 0, width, height, name),
	valPosition (valuePos == LABEL_BOTTOM ? LABEL_BOTTOM : LABEL_TOP),
	valFormat (valueFormat), displayArea ()
{
	valueDisplay.setText (BUtilities::to_string (value, valueFormat));
	valueDisplay.setScrollable (false);
	valueDisplay.setEditable (true);
	valueDisplay.setCallbackFunction(BEvents::EventType::POINTER_DRAG_EVENT, displayDraggedCallback);
	valueDisplay.setCallbackFunction(BEvents::EventType::MESSAGE_EVENT, displayMessageCallback);
	add (valueDisplay);
}

VSliderValue::VSliderValue (const VSliderValue& that) :
		VSlider (that), valueDisplay (that.valueDisplay), valPosition (that.valPosition),
		valFormat (that.valFormat), displayArea (that.displayArea)
{
	add (valueDisplay);
}

VSliderValue& VSliderValue::operator= (const VSliderValue& that)
{
	release (&valueDisplay);

	displayArea = that.displayArea;
	valPosition = that.valPosition;
	valFormat = that.valFormat;
	valueDisplay = that.valueDisplay;
	VSlider::operator= (that);

	add (valueDisplay);

	return *this;
}

Widget* VSliderValue::clone () const {return new VSliderValue (*this);}

void VSliderValue::setValue (const double val)
{
	VSlider::setValue (val);
	valueDisplay.setText(BUtilities::to_string (value, valFormat));
}

void VSliderValue::setValuePosition (const LabelPosition pos)
{
	valPosition = pos;
	update();
}

LabelPosition VSliderValue::getValuePosition () const {return valPosition;}

void VSliderValue::setValueFormat (const std::string& valueFormat)
{
	valFormat = valueFormat;
	update ();
}

std::string VSliderValue::getValueFormat () const {return valFormat;}

Label* VSliderValue::getDisplayLabel () {return &valueDisplay;}

void VSliderValue::update ()
{
	VSlider::update ();

	// Update display
	valueDisplay.moveTo (displayArea.getPosition());
	valueDisplay.resize (displayArea.getExtends());
	if (valueDisplay.getFont ()->getFontSize () != displayArea.getHeight() * 0.8)
	{
		valueDisplay.getFont ()->setFontSize (displayArea.getHeight() * 0.8);
		valueDisplay.update ();
	}
	valueDisplay.setText (BUtilities::to_string (value, valFormat));
}

void VSliderValue::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}
void VSliderValue::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	VSlider::applyTheme (theme, name);
	valueDisplay.applyTheme (theme, name);
}

void VSliderValue::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();

	double dw = (w < h ? w : h);
	double dh = dw / 2.2;
	double dx = getXOffset () + w / 2 - dw / 2;
	double dy = (valPosition == LABEL_TOP ? getYOffset() : getYOffset() + h - dh);
	displayArea = BUtilities::RectArea (dx, dy, dw, dh);

	double h2 = h - displayArea.getHeight();
	double w2 = displayArea.getWidth() / 2;
	knobRadius = (w2 < h2 / 2 ? w2 / 2 : h2 / 4);
	scaleArea = BUtilities::RectArea
	(
		getXOffset () + w / 2 - knobRadius / 2,
		(valPosition == LABEL_TOP ? getYOffset () + displayArea.getHeight() + knobRadius : getYOffset () + knobRadius),
		knobRadius,
		h2 - 2 * knobRadius
	);
	scaleYValue = scaleArea.getY() + (1 - getRelativeValue ()) * scaleArea.getHeight();
	knobPosition = BUtilities::Point (scaleArea.getX() + scaleArea.getWidth() / 2, scaleYValue);
}

void VSliderValue::displayDraggedCallback (BEvents::Event* event)
{
	if (event && event->getWidget())
	{
		BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
		VSliderValue* d = (VSliderValue*)l->getParent();
		if (d && (!l->getEditMode())) d->VSliderValue::onPointerDragged ((BEvents::PointerEvent*)event);
	}
}

void VSliderValue::displayMessageCallback (BEvents::Event* event)
{
	if (event && event->getWidget())
	{
		BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
		VSliderValue* d = (VSliderValue*)l->getParent();
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

			d->setValue (val);
			d->update();
		}
	}
}

}
