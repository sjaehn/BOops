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

#ifndef PADBUTTON_HPP_
#define PADBUTTON_HPP_

#include "BWidgets/Button.hpp"
#include "BWidgets/Focusable.hpp"
#include "BWidgets/Label.hpp"
#include "drawbutton.hpp"
#include "drawSymbol.hpp"

class PadButton : public BWidgets::Button, public BWidgets::Focusable
{
public:
	PadButton	() : PadButton (0.0, 0.0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, "padbutton", NOSYMBOL, 0.0) {}
	PadButton	(const double x, const double y, const double width, const double height,
			 const std::string& name, SymbolIndex symbol = NOSYMBOL, double defaultValue = 0.0) :
		Button (x, y, width, height, name, defaultValue),
		Focusable (std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_IN_MS),
			   std::chrono::milliseconds (BWIDGETS_DEFAULT_FOCUS_OUT_MS)),
		symbol (symbol), fgColors (BWIDGETS_DEFAULT_BGCOLORS), txColors (BWIDGETS_DEFAULT_FGCOLORS),
     		focusLabel_ (0, 0, 40, 20, name_ + BWIDGETS_DEFAULT_FOCUS_NAME, "")
	{
		focusLabel_.setStacking (BWidgets::STACKING_OVERSIZE);
		focusLabel_.setText (symboltxt[symbol + 1]);
   		focusLabel_.resize ();
   		focusLabel_.hide ();
   		add (focusLabel_);
	}

	PadButton (const PadButton& that) :
		Button (that), Focusable (that),
		symbol (that.symbol), fgColors (that.fgColors), txColors (that.txColors),
		focusLabel_ (that.focusLabel_)
	{
		focusLabel_.hide();
		add (focusLabel_);
	}

	PadButton& operator= (const PadButton& that)
	{
		release (&focusLabel_);
		focusLabel_ = that.focusLabel_;
		symbol = that.symbol;
		fgColors = that.fgColors;
		txColors = that.fgColors;
		focusLabel_.hide();

		Widget::operator= (that);
		Focusable::operator= (that);

		add (focusLabel_);

		return *this;

	}

	virtual Widget* clone () const override {return new PadButton (*this);}

	virtual void setSymbol (const SymbolIndex sym)
	{
		symbol = sym;
		focusLabel_.setText (symboltxt[symbol + 1]);
		focusLabel_.resize();
		update();
	}

	virtual void applyTheme (BStyles::Theme& theme) override {applyTheme (theme, name_);}

	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override
	{
		Button::applyTheme (theme, name);
		focusLabel_.applyTheme (theme, name + BWIDGETS_DEFAULT_FOCUS_NAME);
		focusLabel_.resize();

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

	virtual void onFocusIn (BEvents::FocusEvent* event) override
	{
		if (event && event->getWidget())
		{
			if (symbol != NOSYMBOL)
			{
				BUtilities::Point pos = event->getPosition();
				raiseToTop();
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
	SymbolIndex symbol;
	BColors::ColorSet fgColors;
	BColors::ColorSet txColors;
	BWidgets::Label focusLabel_;

	virtual BColors::Color getPadColor ()
	{
		BColors::Color butColor = *fgColors.getColor (getState ());
		butColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_SHADOWED : BWIDGETS_DEFAULT_NORMALLIGHTED);
		return butColor;
	}

	virtual BColors::Color getSymbolColor ()
	{
		BColors::Color symColor = *txColors.getColor (getState ());
		symColor.applyBrightness (getValue() ? BWIDGETS_DEFAULT_SHADOWED : BWIDGETS_DEFAULT_NORMALLIGHTED);
		return symColor;
	}

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

				const double x0 = getXOffset ();
				const double y0 = getYOffset ();
				const double w = getEffectiveWidth ();
				const double h = getEffectiveHeight ();

				const BColors::Color butColor = getPadColor();
				const BColors::Color symColor = getSymbolColor();

				drawButton (cr, x0, y0, w, h, butColor);
				drawSymbol (cr, x0, y0, w, h, symColor, symbol);

				cairo_destroy (cr);
			}
		}
	}
};

#endif /* PADBUTTON_HPP_ */
