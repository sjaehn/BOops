/* Text.cpp
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

#include "Text.hpp"

namespace BWidgets
{
Text::Text () : Text (0.0, 0.0, 0.0, 0.0, "text", "") {}

Text::Text (const std::string& text) : Text (0.0, 0.0, BWIDGETS_DEFAULT_WIDTH, BWIDGETS_DEFAULT_HEIGHT, text, text) {}

Text::Text (const double x, const double y, const double width, const double height, const std::string& text) :
		Text (x, y, width, height, text, text) {}

Text::Text (const double x, const double y, const double width, const double height, const std::string& name, const std::string& text, bool resizable) :
		Widget (x, y, width, height, name),
		textColors (BWIDGETS_DEFAULT_TEXT_COLORS), textFont (BWIDGETS_DEFAULT_FONT), textString (text), yResizable (resizable) {}

Text::Text (const Text& that) : Widget (that)
{
	textColors = that.textColors;
	textFont = that.textFont;
	textString = that.textString;
	yResizable = that.yResizable;
}

Text::~Text () {}

Text& Text::operator= (const Text& that)
{
	textColors = that.textColors;
	textFont = that.textFont;
	textString = that.textString;
	yResizable = that.yResizable;
	Widget::operator= (that);
	if (yResizable) resize (getExtends());
	return *this;
}

Widget* Text::clone () const {return new Text (*this);}

void Text::setText (const std::string& text)
{
	if (text != textString)
	{
		textString = text;
		if (yResizable) resize (getExtends());
		update ();
	}
}
std::string Text::getText () const {return textString;}

void Text::setTextColors (const BColors::ColorSet& colorset)
{
	textColors = colorset;
	update ();
}
BColors::ColorSet* Text::getTextColors () {return &textColors;}

void Text::setFont (const BStyles::Font& font)
{
	textFont = font;
	if (yResizable) resize (getExtends());
	update ();
}
BStyles::Font* Text::getFont () {return &textFont;}

void Text::setYResizable (const bool resizable) {yResizable = resizable;}

bool Text::isYResizable () const {return yResizable;}

void Text::setWidth (const double width)
{
	Widget::setWidth (width);
	if (yResizable) resize (getExtends());
}

void Text::resize () {resize (getExtends());}

void Text::resize (const double width, const double height) {resize (BUtilities::Point (width, height));}

void Text::resize (const BUtilities::Point extends)
{
	if (yResizable)
	{
		double ySize = getTextBlockHeight (getTextBlock()) + 2 * getYOffset();
		Widget::resize (BUtilities::Point (extends.x, ySize));
	}
	else Widget::resize (extends);
}

void Text::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void Text::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);

	// Color
	void* colorsPtr = theme.getStyle(name, BWIDGETS_KEYWORD_TEXTCOLORS);
	if (colorsPtr) textColors = *((BColors::ColorSet*) colorsPtr);

	// Font
	void* fontPtr = theme.getStyle(name, BWIDGETS_KEYWORD_FONT);
	if (fontPtr) setFont (*((BStyles::Font*) fontPtr));

	else if (colorsPtr) update ();
}

std::vector<std::string> Text::getTextBlock ()
{
	std::vector<std::string> textblock;
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();
	cairo_t* cr = cairo_create (widgetSurface_);
	cairo_text_decorations decorations;
	strncpy (decorations.family, textFont.getFontFamily ().c_str (), 63);
	decorations.size = textFont.getFontSize ();
	decorations.slant = textFont.getFontSlant ();
	decorations.weight = textFont.getFontWeight ();

	char* textCString = (char*) malloc (strlen (textString.c_str ()) + 1);
	if (textCString)
	{
		strcpy (textCString, textString.c_str ());

		for (double y = 0; (yResizable || (y <= h)) && (strlen (textCString) > 0); )
		{
			char* outputtext = cairo_create_text_fitted (cr, w, decorations, textCString);
			if (outputtext[0] == '\0') break;
			cairo_text_extents_t ext = textFont.getTextExtents(cr, outputtext);
			textblock.push_back (std::string (outputtext));
			y += (ext.height * textFont.getLineSpacing ());
			cairo_text_destroy (outputtext);
		}

		free (textCString);
	}

	cairo_destroy (cr);
	return textblock;
}

double Text::getTextBlockHeight (std::vector<std::string> textBlock)
{
	double blockheight = 0.0;
	cairo_t* cr = cairo_create (widgetSurface_);

	for (std::string textline : textBlock)
	{
		//cairo_text_extents_t ext = textFont.getTextExtents(cr, textline.c_str ());
		blockheight += (textFont.getFontSize () * textFont.getLineSpacing ());
	}

	cairo_destroy (cr);
	return blockheight;
}

void Text::draw (const BUtilities::RectArea& area)
{
	if ((!widgetSurface_) || (cairo_surface_status (widgetSurface_) != CAIRO_STATUS_SUCCESS)) return;

	// Draw super class widget elements first
	Widget::draw (area);

	cairo_t* cr = cairo_create (widgetSurface_);

	if (cairo_status (cr) == CAIRO_STATUS_SUCCESS)
	{
		// Limit cairo-drawing area
		cairo_rectangle (cr, area.getX (), area.getY (), area.getWidth (), area.getHeight ());
		cairo_clip (cr);

		double xoff = getXOffset ();
		double yoff = getYOffset ();
		double w = getEffectiveWidth ();
		double h = getEffectiveHeight ();

		// textString -> textblock
		std::vector<std::string> textblock = getTextBlock ();
		double blockheight = getTextBlockHeight (textblock);

		// Calculate vertical alignment of the textblock
		double y0 = 0;
		switch (textFont.getTextVAlign ())
		{
		case BStyles::TEXT_VALIGN_TOP:		y0 = 0;
							break;
		case BStyles::TEXT_VALIGN_MIDDLE:	y0 = h / 2 - blockheight / 2;
							break;
		case BStyles::TEXT_VALIGN_BOTTOM:	y0 = h - blockheight;
							break;
		default:				y0 = 0;
		}


		// Output of textblock
		BColors::Color lc = *textColors.getColor (BColors::NORMAL);
		cairo_set_source_rgba (cr, lc.getRed (), lc.getGreen (), lc.getBlue (), lc.getAlpha ());
		cairo_select_font_face (cr, textFont.getFontFamily ().c_str (), textFont.getFontSlant (), textFont.getFontWeight ());
		cairo_set_font_size (cr, textFont.getFontSize ());
		double ycount = 0.0;

		for (std::string textline : textblock)
		{
			cairo_text_extents_t ext = textFont.getTextExtents (cr, textline);

			double x0;
			switch (textFont.getTextAlign ())
			{
			case BStyles::TEXT_ALIGN_LEFT:		x0 = - ext.x_bearing;
								break;
			case BStyles::TEXT_ALIGN_CENTER:	x0 = w / 2 - ext.width / 2 - ext.x_bearing;
								break;
			case BStyles::TEXT_ALIGN_RIGHT:		x0 = w - ext.width - ext.x_bearing;
								break;
			default:				x0 = 0;
			}

			cairo_move_to (cr, xoff + x0, yoff + y0 + ycount - ext.y_bearing);
			cairo_show_text (cr, textline.c_str ());
			ycount += (textFont.getFontSize () * textFont.getLineSpacing ());
		}
	}

	cairo_destroy (cr);
}

}
