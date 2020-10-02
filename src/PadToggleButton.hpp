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

#ifndef PADTOGGLEBUTTON_HPP_
#define PADTOGGLEBUTTON_HPP_

#include "BWidgets/ToggleButton.hpp"
#include "drawbutton.hpp"
#include "drawSymbol.hpp"

class PadToggleButton : public BWidgets::ToggleButton
{
public:
	PadToggleButton () : PadToggleButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "padbutton", NOSYMBOL, 0.0) {}
	PadToggleButton (const double x, const double y, const double width, const double height, const std::string& name, SymbolIndex symbol = NOSYMBOL, double defaultValue = 0.0) :
		ToggleButton (x, y, width, height, name, defaultValue),
		symbol (symbol), fgColors (BWIDGETS_DEFAULT_BGCOLORS), txColors (BWIDGETS_DEFAULT_FGCOLORS) {}

	virtual Widget* clone () const override {return new PadToggleButton (*this);}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		ToggleButton::applyTheme (theme, name);

		void* fgPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FGCOLORS);
		if (fgPtr)
		{
			fgColors = *((BColors::ColorSet*) fgPtr);
			update ();
		}

		void* txPtr = theme.getStyle(name, BWIDGETS_KEYWORD_TEXTCOLORS);
		if (txPtr)
		{
			txColors = *((BColors::ColorSet*) txPtr);
			update ();
		}
	}

protected:
	SymbolIndex symbol;
	BColors::ColorSet fgColors;
	BColors::ColorSet txColors;

	virtual void draw (const BUtilities::RectArea& area) override
	{
		if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

		if ((getWidth () >= 6) && (getHeight () >= 6))
		{
			// Draw super class widget elements first
			Widget::draw (area);

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

				BColors::Color butColor = *fgColors.getColor (getState ());
				butColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_NORMALLIGHTED : BWIDGETS_DEFAULT_SHADOWED);
				BColors::Color symColor = *txColors.getColor (getState ());
				symColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_NORMALLIGHTED : BWIDGETS_DEFAULT_SHADOWED);

				drawButton (cr, x0, y0, w, h, butColor);
				drawSymbol (cr, x0, y0, w, h, symColor, symbol);

				cairo_destroy (cr);
			}
		}
	}
};

#endif /* PADTOGGLEBUTTON_HPP_ */
