/* BStyles.hpp
 * Copyright (C) 2018  Sven Jähnichen
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

#ifndef BSTYLES_HPP_
#define BSTYLES_HPP_

#include <stdint.h>
#include <cstring>
#include <string>
#include <cairo/cairo.h>
#include "cairoplus.h"
#include <iostream>

#include "BColors.hpp"

#define STYLEPTR(ptr) (void*) (ptr)

namespace BStyles
{
/**
 * Class BStyles::Line
 *
 * Line style class. A BStyles::Line is defined by its color, width and
 * TODO dashes
 */
class Line
{
public:
	Line ();
	Line (const BColors::Color& color, const double width);

	/**
	 * Sets color of the line style
	 * @param color BColors::Color
	 */
	void setColor (const BColors::Color& color);

	/**
	 * Gets (a pointer to the) color of the line style
	 * @return Returns BColors::Color.
	 */
	BColors::Color* getColor ();

	/**
	 * Sets width of the line
	 * @param width Width of the line in pt.
	 */
	void setWidth (const double width);

	/**
	 * Gets width of the line
	 * @return Width of the line in pt.
	 */
	double getWidth () const;

private:
	BColors::Color lineColor;
	int lineWidth;
};
/*
 * End of class BWidgets::Line
 *****************************************************************************/

const Line blackLine1pt = Line (BColors::black, 1.0);
const Line whiteLine1pt = Line (BColors::white, 1.0);
const Line greyLine1pt = Line (BColors::grey, 1.0);
const Line lightgreyLine1pt = Line (BColors::lightgrey, 1.0);
const Line noLine = Line (BColors::invisible, 0.0);


/**
 * Class BStyles::Border
 *
 * Border style class. A BStyles::Border is defined by its margin, padding,
 * line and corner radius
 */
class Border
{
public:
	Border ();
	Border (const Line& line);
	Border (const Line& line, const double margin, const double padding);
	Border (const Line& line, const double margin, const double padding, const double radius);

	/**
	 * Sets the line of a border style
	 * @param line BStyles::Line
	 */
	void setLine (const Line& line);

	/**
	 * Gets a pointer to the line of a border style
	 * @return BStyles::Line
	 */
	Line* getLine ();

	/**
	 * Sets the margin of a border style
	 * @param margin Distance from border line to the outer limits of the
	 * 				 border.
	 */
	void setMargin (const double margin);

	/**
	 * Gets the margin of a border style
	 * @return Distance from border line to the outer limits of the border.
	 */
	double getMargin () const;

	/**
	 * Sets the padding of a border style
	 * @param padding Distance from border line to the inner limits of the
	 * 				  border.
	 */
	void setPadding (const double padding);

	/**
	 * Gets the padding of a border style
	 * @return Distance from border line to the inner limits of the border.
	 */
	double getPadding () const;

	/**
	 * Sets the radius of the corners of a border style
	 * @param radius Radius of the corners. If (radius != 0), the corners will
	 * 				 be rounded.
	 */
	void setRadius (const double radius);

	/**
	 * Gets the radius of the corners of a border style
	 * @param radius Radius of the corners.
	 */
	double getRadius () const;

protected:
	Line borderLine;
	double borderMargin;
	double borderPadding;
	double borderRadius;
};
/*
 * End of class BWidgets::Border
 *****************************************************************************/

const Border blackBorder1pt = Border (blackLine1pt);
const Border whiteBorder1pt = Border (whiteLine1pt);
const Border greyBorder1pt = Border (greyLine1pt);
const Border lightgreyBorder1pt = Border (lightgreyLine1pt);
const Border noBorder = Border (noLine);



/**
 * Class BStyles::Fill
 *
 * Fill style class. A BStyles::Fill can be a plain color or a Cairo image
 * surface (higher priority).
 */
class Fill
{
public:
	Fill ();
	Fill (const BColors::Color& color);
	Fill (const std::string& filename);
	Fill (const Fill& that);
	Fill& operator= (const Fill& that);
	~Fill ();

	/**
	 * Sets the color of a fill style
	 * @param color BColors::Color
	 */
	void setColor (const BColors::Color& color);

	/**
	 * Gets (a pointer to) the color of a fill style
	 * @return BColors::Color
	 */
	BColors::Color* getColor ();

	/**
	 * Loads a PNG file into a Cairo image surface and uses it for the fill
	 * style.
	 * @param filename File name of the PNG file
	 */
	void loadFillFromFile (const std::string& filename);

	/**
	 * Copies a Cairo image surface and uses it for the fill style.
	 * @param surface Pointer to the source cairo image surface
	 */
	void loadFillFromCairoSurface (cairo_surface_t* surface);

	//TODO purgeFillSurface ();

	/**
	 * Gets a pointer to the cairo image surface used for the fill style.
	 * @return Pointer to a cairo image surface or nullptr if no surface is
	 * 		   used (plain color mode).
	 */
	cairo_surface_t* getCairoSurface ();


protected:
	BColors::Color fillColor;
	cairo_surface_t* fillSurface;

};
/*
 * End of class BWidgets::Fill
 *****************************************************************************/

const Fill blackFill = Fill (BColors::black);
const Fill whiteFill = Fill (BColors::white);
const Fill redFill = Fill (BColors::red);
const Fill greenFill = Fill (BColors::green);
const Fill blueFill = Fill (BColors::blue);
const Fill greyFill = Fill (BColors::grey);
const Fill darkgreyFill = Fill (BColors::darkgrey);
const Fill grey20Fill = Fill (BColors::grey20);
const Fill noFill = Fill (BColors::invisible);

typedef enum {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT,
} TextAlign;

typedef enum {
	TEXT_VALIGN_TOP,
	TEXT_VALIGN_MIDDLE,
	TEXT_VALIGN_BOTTOM
} TextVAlign;

/**
 * Class BStyles::Font
 *
 * Font style class. A BStyles::Font is defined by the Cairo font properties
 * font family, slant, weight and its size
 */
class Font
{
public:
	Font ();
	Font (const std::string& family, const cairo_font_slant_t slant, const cairo_font_weight_t weight, const double size,
		  TextAlign align = TEXT_ALIGN_LEFT, TextVAlign valign = TEXT_VALIGN_TOP, double lineSpacing = 1.25);

	/**
	 * Sets font family of the font style
	 * @param family Cairo font family name (as std::string)
	 */
	void setFontFamily (const std::string& family);

	/**
	 * Gets font family of the font style
	 * @return Cairo font family name (as std::string)
	 */
	std::string getFontFamily () const;

	/**
	 * Sets font slant of the font style
	 * @param slant Cairo font slant
	 */
	void setFontSlant (const cairo_font_slant_t slant);

	/**
	 * Gets font slant of the font style
	 * @return Cairo font slant
	 */
	cairo_font_slant_t getFontSlant () const;

	/**
	 * Sets font weight of the font style
	 * @param weight Cairo font weight
	 */
	void setFontWeight (const cairo_font_weight_t weight);

	/**
	 * Gets font weight of the font style
	 * @return Cairo font weight
	 */
	cairo_font_weight_t getFontWeight () const;

	/**
	 * Sets the horizonal text alignment of the font style
	 * @param align Enum of text alignment
	 */
	void setTextAlign (const TextAlign align);

	/**
	 * Gets the horizonal text alignment of the font style
	 * @return Enum of text alignment
	 */
	TextAlign getTextAlign () const;

	/**
	 * Sets the vertical text alignment of the font style
	 * @param align Enum of text alignment
	 */
	void setTextVAlign (const TextVAlign valign);

	/**
	 * Gets the vertical text alignment of the font style
	 * @return Enum of text alignment
	 */
	TextVAlign getTextVAlign () const;

	/**
	 * Sets line spacing of the font style
	 * @param lineSpacing Line spacing factor
	 */
	void setLineSpacing (const double lineSpacing);

	/**
	 * Gets line spacing of the font style
	 * @return Line spacing factor
	 */
	double getLineSpacing () const;

	/**
	 * Sets font size of the font style
	 * @param size Font size as used by Cairo
	 */
	void setFontSize (const double size);

	/**
	 * Gets font size of the font style
	 * @return Font size as used by Cairo
	 */
	double getFontSize () const;

	/**
	 * Gets the output dimensions of a text
	 * @param cr Pointer to a Cairo context that will be used to calculate the
	 * 			 output dimensions of the text.
	 * @param text Text to calculate output dimensions for.
	 * @return Output dimensions as Cairo text extents.
	 */
	cairo_text_extents_t getTextExtents (cairo_t* cr, const std::string& text) const;

private:
	std::string fontFamily;
	cairo_font_slant_t fontSlant;
	cairo_font_weight_t fontWeight;
	double fontSize;
	TextAlign textAlign;
	TextVAlign textVAlign;
	double textLineSpacing;
};

/*
 * End of class BWidgets::Font
 *****************************************************************************/

const Font sans12pt = Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0);


/**
 * Struct BStyles::Style
 *
 * A style is defined by its name and (a pointer to) a style element (e.g.,
 * BColors::Color, BColors::ColorSet, BStyle::Line, BStyle::Border,
 * BStyle::Fill, BStyle::Font).
 */
typedef struct {
	std::string name;
	void* stylePtr;
} Style;

/**
 * Class BStyles::StyleSet
 *
 * A BStyles::StyleSet is a collection of different styles (and therefore
 * different style elements). The BStyles::StyleSet itself is defined by its
 * name. A BStyles::StyleSet is intended to be used for an object (like a
 * BWidgets::Widget) to set up its properties.
 *
 * For example, a BStyles::StyleSet can look like this:
 * {"example", {{"color", &BColors::white},
 * 				{"border", &BStyles::noBorder},
 * 				{"font", &BStyles::sans12pt}
 * 			   }
 * }
 *
 * Feature:
 * The style name "uses" means that another BStyles::StyleSet (to which
 * Style.ptr points to) is used to this (pre-)define BStyles::StyleSet.
 */
class StyleSet {
public:
	StyleSet ();
	StyleSet (const std::string& name, const std::vector<Style>& styles);

	/**
	 * Adds a style to (or overwrites an existing of) the BStyles::StyleSet.
	 * @param styleName Name of the style to be added or overwritten.
	 * @param ptr Pointer to the style element.
	 */
	void addStyle (const std::string& styleName, void* ptr);

	//TODO void addStyle (Style& style);

	/**
	 * Removes an existing style from the BStyles::StyleSet.
	 * @param styleName Name of the style to be removed from the
	 * 					BStyles::StyleSet.
	 * TODO throws style doesn't exist
	 */
	void removeStyle (const std::string& styleName);

	/**
	 * Gets a pointer to an existing style from the BStyles::StyleSet.
	 * @param styleName Name of the style.
	 * TODO throws style doesn't exist
	 */
	void* getStyle (const std::string& styleName);

	/**
	 * Sets the name of the BStyles::StyleSet
	 * @param name Name of the BStyles::StyleSet.
	 */
	void setName (const std::string& name);

	/**
	 * Gets the name of the BStyles::StyleSet
	 * @return Name of the BStyles::StyleSet
	 */
	std::string getName () const;

protected:
	std::string stylesetName;
	std::vector<Style> styleVector;
};
/*
 * End of class BWidgets::StyleSet
 *****************************************************************************/

/**
 * Class BStyles::Theme
 *
 * A BStyles::Theme is a collection of different BStyles::StyleSets. A theme is
 * intended to be used set up all properties of objects (like a GUI based on
 * BWidgets) at once.
 */
class Theme
{
public:
	Theme ();
	Theme (const std::vector<StyleSet>& theme);

	/**
	 * Adds (or overwrites an existing) style to an BStyles::StyleSet
	 * within the Theme. If the respective BStyles::StyleSet doesn't
	 * exist yet, it will be created.
	 * @param setName Name of the BStyles::StyleSet
	 * @param styleName Name of the BStyles::Style
	 * @param ptr Pointer to the style element
	 */
	void addStyle (const std::string& setName, const std::string& styleName, void* ptr);

	//TODO void addStyle (Style style);
	//TODO void addStyleSet (StyleSet styleSet);
	//TODO StyleSet* getStyleSet (std::string setName);

	/**
	 * Removes an existing style.
	 * @param setName Name of the BStyles::StyleSet
	 * @param styleName Name of the style to be removed.
	 * TODO throws style doesn't exist
	 */
	void removeStyle (const std::string& setName, const std::string& styleName);

	/**
	 * Gets an existing style.
	 * @param setName Name of the BStyles::StyleSet
	 * @param styleName Name of the style to be removed.
	 * TODO throws style doesn't exist
	 */
	void* getStyle (const std::string& setName, const std::string& styleName);

protected:
	std::vector<StyleSet> stylesetVector;
};
/*
 * End of class BWidgets::Theme
 *****************************************************************************/

}

#endif /* BSTYLES_HPP_ */
