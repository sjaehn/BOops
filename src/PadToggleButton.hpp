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

#ifndef PADTOGGLEBUTTON_HPP_
#define PADTOGGLEBUTTON_HPP_

#include "PadButton.hpp"

class PadToggleButton : public PadButton
{
public:
	PadToggleButton () : PadToggleButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "padbutton", NOSYMBOL, 0.0) {}
	PadToggleButton (const double x, const double y, const double width, const double height, const std::string& name, SymbolIndex symbol = NOSYMBOL, double defaultValue = 0.0) :
		PadButton (x, y, width, height, name, symbol, defaultValue)
	{}

	virtual Widget* clone () const override {return new PadToggleButton (*this);}

	void onButtonPressed (BEvents::PointerEvent* event)
	{
		if (getValue ()) setValue (0.0);
		else setValue (1.0);

		Widget::cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
	}

	void onButtonReleased (BEvents::PointerEvent* event)
	{
		Widget::cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);
	}

protected:
	virtual BColors::Color getPadColor () override
	{
		BColors::Color butColor = *fgColors.getColor (getState ());
		butColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_NORMALLIGHTED : BWIDGETS_DEFAULT_SHADOWED);
		return butColor;
	}

	virtual BColors::Color getSymbolColor () override
	{
		BColors::Color symColor = *txColors.getColor (getState ());
		symColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_NORMALLIGHTED : BWIDGETS_DEFAULT_SHADOWED);
		return symColor;
	}
};

#endif /* PADTOGGLEBUTTON_HPP_ */
