/* BStyles.cpp
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

#include "BStyles.hpp"

namespace BStyles
{

/*****************************************************************************
 * Class BStyles::Line
 *****************************************************************************/

Line::Line () : Line (BColors::invisible, 0.0) {}
Line::Line (const BColors::Color& color, const double width) : lineColor (color), lineWidth (width) {}
void Line::setColor (const BColors::Color& color) {lineColor = color;}
BColors::Color* Line::getColor () {return &lineColor;}
void Line::setWidth (const double width) {lineWidth = width;}
double Line::getWidth () const {return lineWidth;}

/*
 * End of class BWidgets::Line
 *****************************************************************************/


/*****************************************************************************
 * Class BStyles::Border
 *****************************************************************************/

Border::Border () : Border (noLine, 0.0, 0.0, 0.0) {}
Border::Border (const Line& line) : Border (line, 0.0, 0.0, 0.0) {}
Border::Border (const Line& line, const double margin, const double padding) : Border (line, margin, padding, 0.0) {}
Border::Border (const Line& line, const double margin, const double padding, const double radius) :
		borderLine (line), borderMargin (margin), borderPadding (padding), borderRadius (radius) {}

void Border::setLine (const Line& line) {borderLine = line;}
Line* Border::getLine () {return &borderLine;}
void Border::setMargin (const double margin) {borderMargin = margin;}
double Border::getMargin () const {return borderMargin;}
void Border::setPadding (const double padding) {borderPadding = padding;}
double Border::getPadding () const {return borderPadding;}
void Border::setRadius (const double radius) {borderRadius = radius;}
double Border::getRadius () const {return borderRadius;}

/*
 * End of class BWidgets::Border
 *****************************************************************************/


/*****************************************************************************
 * Class BStyles::Fill
 *****************************************************************************/

Fill::Fill () : fillColor (BColors::invisible), fillSurface (nullptr) {}
Fill::Fill (const BColors::Color& color) : fillColor (color), fillSurface (nullptr) {}
Fill::Fill (const std::string& filename) : fillColor (BColors::invisible), fillSurface (nullptr)
{
	loadFillFromFile (filename);
}

Fill::Fill (const Fill& that)
{
	fillColor = that.fillColor;
	if (that.fillSurface) fillSurface = cairo_image_surface_clone_from_image_surface (that.fillSurface);
	else fillSurface = nullptr;
}

Fill& Fill::operator= (const Fill& that)
{
	if (this != &that)
	{
		fillColor = that.fillColor;
		if (that.fillSurface) fillSurface = cairo_image_surface_clone_from_image_surface (that.fillSurface);
		else fillSurface = nullptr;
	}

	return *this;
}

Fill::~Fill ()
{
	if (fillSurface && (cairo_surface_status (fillSurface) == CAIRO_STATUS_SUCCESS)) cairo_surface_destroy (fillSurface);
}

void Fill::setColor (const BColors::Color& color) {fillColor = color;}

BColors::Color* Fill::getColor () {return &fillColor;}

void Fill::loadFillFromFile (const std::string& filename)
{
	if (fillSurface) cairo_surface_destroy (fillSurface);
	fillSurface = cairo_image_surface_create_from_png (filename.c_str());
}

void Fill::loadFillFromCairoSurface (cairo_surface_t* surface)
{
	if (fillSurface) cairo_surface_destroy (fillSurface);
	fillSurface = cairo_image_surface_clone_from_image_surface (surface);
}

cairo_surface_t* Fill::getCairoSurface () {return fillSurface;}

/*
 * End of class BWidgets::Fill
 *****************************************************************************/


/*****************************************************************************
 * Class BStyles::Font
 *****************************************************************************/

Font::Font () : Font ("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL, 12.0) {}
Font::Font (const std::string& family, const cairo_font_slant_t slant, const cairo_font_weight_t weight, const double size,
			TextAlign align, TextVAlign valign, double lineSpacing)
	: fontFamily (family), fontSlant (slant), fontWeight (weight), fontSize (size),
	  textAlign (align), textVAlign (valign), textLineSpacing (lineSpacing) {}

void Font::setFontFamily (const std::string& family) {fontFamily = family;}
std::string Font::getFontFamily () const {return fontFamily;}
void Font::setFontSlant (const cairo_font_slant_t slant) {fontSlant = slant;}
cairo_font_slant_t Font::getFontSlant () const {return fontSlant;}
void Font::setFontWeight (const cairo_font_weight_t weight) {fontWeight = weight;}
cairo_font_weight_t Font::getFontWeight () const {return fontWeight;}
void Font::setTextAlign (const TextAlign align) {textAlign = align;}
TextAlign Font::getTextAlign () const {return textAlign;}
void Font::setTextVAlign (const TextVAlign valign) {textVAlign = valign;}
TextVAlign Font::getTextVAlign () const {return textVAlign;}
void Font::setLineSpacing (const double lineSpacing) {textLineSpacing = lineSpacing;}
double Font::getLineSpacing () const {return textLineSpacing;}
void Font::setFontSize (const double size) {fontSize = size;}
double Font::getFontSize () const {return fontSize;}

cairo_text_extents_t Font::getTextExtents (cairo_t* cr, const std::string& text) const
{
	if (cr && (! cairo_status (cr)))
	{
		cairo_save (cr);

		cairo_text_extents_t ext;
		cairo_select_font_face (cr, fontFamily.c_str (), fontSlant, fontWeight);
		cairo_set_font_size (cr, fontSize);
		cairo_text_extents (cr, text.c_str(), &ext);

		cairo_restore (cr);
		return ext;
	}
	else
	{
		return {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	}
}

/*
 * End of class BWidgets::Font
 *****************************************************************************/


/*****************************************************************************
 * Class BStyles::StyleSet
 *****************************************************************************/

StyleSet::StyleSet () {}
StyleSet::StyleSet (const std::string& name, const std::vector<Style>& styles) : stylesetName (name), styleVector (styles) {}

void StyleSet::addStyle (const std::string& styleName, void* ptr)
{
	for (Style style : styleVector)
	{
		if (style.name == styleName)
		{
			// Overwrite existing style
			std::cerr << "Msg from BStyles::StyleSet::addStyle(): Overwrite existing " << stylesetName << "/" << styleName
					  << "." << std:: endl;
			style.stylePtr = ptr;
			return;
		}
	}

	// No hit for styleName? Append style to existing styleset
	Style newStyle = {styleName, ptr};
	styleVector.push_back (newStyle);
	return;
}

void StyleSet::removeStyle (const std::string& styleName)
{
	for (std::vector<Style>::iterator it = styleVector.begin (); it !=styleVector.end (); ++it)
	{
		if (it->name == styleName) // @suppress("Field cannot be resolved")
		{
			// Delete existing style
			styleVector.erase (it);
			return;
		}
	}

	// No hit?
	std::cerr << "Msg from BStyles::StyleSet::removeStyle(): " << stylesetName << "/" << styleName
			  << " doesn't exist. Nothing to delete." << std:: endl;
	return;
}

void* StyleSet::getStyle (const std::string& styleName)
{
	void* ptr = nullptr;
	for (Style style : styleVector)
	{
		if (style.name == "uses")
		{
			StyleSet* usedSet = (StyleSet*) style.stylePtr;
			ptr = usedSet->getStyle (styleName);

		}
		if (style.name == styleName)
		{
			return ptr = style.stylePtr;
		}
	}

	// No hit?
	// if (!ptr) std::cerr << "Msg from BStyles::StyleSet::getStyle(): " << stylesetName << "/" << styleName << " doesn't exist." << std:: endl;
	return ptr;
}

void StyleSet::setName (const std::string& name) {stylesetName = name;}
std::string StyleSet::getName () const {return stylesetName;}

/*
 * End of class BWidgets::StyleSet
 *****************************************************************************/

/*****************************************************************************
 * Class BStyles::Theme
 *****************************************************************************/

Theme::Theme () {};
Theme::Theme (const std::vector<StyleSet>& theme): stylesetVector (theme) {};

void Theme::addStyle (const std::string& setName, const std::string& styleName, void* ptr)
{
	for (StyleSet styleSet : stylesetVector)
	{
		if (styleSet.getName () == setName)
		{
			styleSet.addStyle (styleName, ptr);
			return;
		}
	}

	// No hit for styleset? Append styleset to existing theme
	StyleSet newSet = {setName, {{styleName, ptr}}};
	stylesetVector.push_back (newSet);
}

void Theme::removeStyle (const std::string& setName, const std::string& styleName)
{
	for (StyleSet styleSet : stylesetVector)
	{
		if (styleSet.getName () == setName)
		{
			styleSet.removeStyle (styleName);
			return;
		}
	}

	// No hit?
	// std::cerr << "Msg from BStyles::Theme::removeStyle(): " << setName << "/" << styleName
	//		     << " doesn't exist. Nothing to delete." << std:: endl;
	return;
}

void* Theme::getStyle (const std::string& setName, const std::string& styleName)
{
	for (StyleSet styleSet : stylesetVector)
	{
		if (styleSet.getName () == setName)
		{
			return styleSet.getStyle (styleName);
		}
	}

	// No hit?
	// std::cerr << "Msg from BStyles::Theme::getStyle(): " << setName << "/" << styleName
	// 		     << " doesn't exist." << std:: endl;
	return nullptr;
}

/*
 * End of class BWidgets::Theme
 *****************************************************************************/


}
