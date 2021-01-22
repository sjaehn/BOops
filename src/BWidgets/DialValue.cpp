/* DialValue.cpp
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

#include "DialValue.hpp"
#include "../BUtilities/to_string.hpp"
#include "../BUtilities/stof.hpp"

namespace BWidgets
{

DialValue::DialValue () :
	DialValue
	(
		0.0, 0.0, BWIDGETS_DEFAULT_DIALVALUE_WIDTH, BWIDGETS_DEFAULT_DIALVALUE_HEIGHT,
		"dialvalue",
		BWIDGETS_DEFAULT_VALUE, BWIDGETS_DEFAULT_RANGE_MIN, BWIDGETS_DEFAULT_RANGE_MAX, BWIDGETS_DEFAULT_RANGE_STEP,
		BWIDGETS_DEFAULT_VALUE_FORMAT
	) {}

DialValue::DialValue (const double x, const double y, const double width, const double height, const std::string& name,
		const double value, const double min, const double max, const double step,
		const std::string& valueFormat) :
	Dial (x, y, width, height, name, value, min, max, step),
	valueDisplay(0, 0.75 * height, width, 0.25 * height, name),
	valFormat (valueFormat)
{
	valueDisplay.setText (BUtilities::to_string (value, valueFormat));
	valueDisplay.setScrollable (false);
	valueDisplay.setEditable (true);
	valueDisplay.setCallbackFunction(BEvents::EventType::POINTER_DRAG_EVENT, displayDraggedCallback);
	valueDisplay.setCallbackFunction(BEvents::EventType::MESSAGE_EVENT, displayMessageCallback);
	add (valueDisplay);
}

DialValue::DialValue (const DialValue& that) :
		Dial (that), valueDisplay (that.valueDisplay), valFormat (that.valFormat)
{
	add (valueDisplay);
}

DialValue::~DialValue () {}

DialValue& DialValue::operator= (const DialValue& that)
{
	release (&valueDisplay);
	valFormat = that.valFormat;
	Dial::operator= (that);
	valueDisplay = that.valueDisplay;
	add (valueDisplay);

	return *this;
}

Widget* DialValue::clone () const {return new DialValue (*this);}

void DialValue::setValue (const double val)
{
	Dial::setValue (val);
	valueDisplay.setText(BUtilities::to_string (value, valFormat));
}

void DialValue::setValueFormat (const std::string& valueFormat)
{
	valFormat = valueFormat;
	update ();
}

std::string DialValue::getValueFormat () const {return valFormat;}

Label* DialValue::getDisplayLabel () {return &valueDisplay;}

void DialValue::update ()
{
	Dial::update();

	// Update display
	valueDisplay.moveTo (dialCenter.x - dialRadius, dialCenter.y + 0.7 * dialRadius);
	valueDisplay.setWidth (2 * dialRadius);
	valueDisplay.setHeight (0.5 * dialRadius);
	if (valueDisplay.getFont ()->getFontSize () != 0.4 * dialRadius)
	{
		valueDisplay.getFont ()->setFontSize (0.4 * dialRadius);
		valueDisplay.update ();
	}
	valueDisplay.setText (BUtilities::to_string (value, valFormat));

}

void DialValue::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}
void DialValue::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Dial::applyTheme (theme, name);
	valueDisplay.applyTheme (theme, name);
	update ();
}

void DialValue::updateCoords ()
{
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();
	dialRadius = (w < h / 1.2 ? w / 2 : h / 2.4);
	dialCenter.x = getWidth () / 2;
	dialCenter.y = getHeight () / 2 - 0.2 * dialRadius;
}

void DialValue::displayDraggedCallback (BEvents::Event* event)
{
	if (event && event->getWidget())
	{
		BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
		DialValue* d = (DialValue*)l->getParent();
		if (d && (!l->getEditMode())) d->DialValue::onPointerDragged ((BEvents::PointerEvent*)event);
	}
}

void DialValue::displayMessageCallback (BEvents::Event* event)
{
	if (event && event->getWidget())
	{
		BWidgets::Label* l = (BWidgets::Label*)event->getWidget();
		DialValue* d = (DialValue*)l->getParent();
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
