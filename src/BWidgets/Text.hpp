/* Text.hpp
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

#ifndef BWIDGETS_TEXT_HPP_
#define BWIDGETS_TEXT_HPP_

#include "Widget.hpp"

namespace BWidgets
{

/**
 * Class BWidgets::Text
 *
 * Text output widget with line breaks. Line breaks occur
 * (i) on "\n" or
 * (ii) on spaces when text length exceed the widget width or
 * (iii) on any position when text length exceed the widget width.
 */
class Text : public Widget
{
public:
	Text ();
	Text (const std::string& text);
	Text (const double x, const double y, const double width, const double height, const std::string& text);
	Text (const double x, const double y, const double width, const double height, const std::string& name, const std::string& text, bool resizable = false);

	/**
	 * Creates a new (orphan) text widget and copies the text widget properties
	 * from a source text widget. This method doesn't copy any parent or child
	 * widgets.
	 * @param that Source text widget
	 */
	Text (const Text& that);

	~Text ();

	/**
	 * Assignment. Copies the text widget properties from a source text widget
	 *  and keeps its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the text widget is visible.
	 * @param that Source text widget
	 */
	Text& operator= (const Text& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Sets the output text.
	 * @param text Output text
	 */
	void setText (const std::string& text);

	/**
	 * Gets the output text
	 * @return Output text
	 */
	std::string getText () const;

	/**
	 * Sets the BColors::ColorSet for this widget
	 * @param colors Color set.
	 */
	void setTextColors (const BColors::ColorSet& colorset);

	/**
	 * Gets (a pointer to) the BColors::ColorSet of this widget.
	 * @return Pointer to the color set.
	 */
	BColors::ColorSet* getTextColors ();

	/**
	 * Sets the font for the text output.
	 * @param font Font
	 */
	void setFont (const BStyles::Font& font);

	/**
	 * Gets (a pointer to) the font for the text output.
	 * @return Pointer to font
	 */
	BStyles::Font* getFont ();

	/**
	 * Sets whether the widget resizes to the text block size or not.
	 * @param resizable True, if the widget resizes.
	 */
	void setYResizable (const bool resizable);

	/**
	 * Gets whether the widget resizes to the text block size or not.
	 * @param return True, if the widget resizes.
	 */
	bool isYResizable () const;

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible.
	 * @param width New widgets width
	 */
	virtual void setWidth (const double width) override;

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible. If no parameters are given, the widget will be
	 * resized to the size of the containing child widgets.
	 * @param width		New widgets width
	 * @param height	New widgets height
	 * @param extends	New widget extends
	 */
	virtual void resize () override;
	virtual void resize (const double width, const double height) override;
	virtual void resize (const BUtilities::Point extends) override;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				Styles used are:
	 * 				"textcolors" for BColors::ColorSet
	 * 				"font" for BStyles::Font
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Gets a block (a vector) of text lines that fit into the widget output.
	 * If the widget is not resizable: the text is clipped, when lines exceed
	 * the widget height. If the widget is resizable, the whole text will be
	 * returned as a block of lines.
	 * @return	Vector of text lines
	 */
	std::vector<std::string> getTextBlock ();

	/**
	 * Gets the height of a given text block as calculated using Cairo.
	 * @param textBlock Vector of text lines
	 * @return 			Text block height.
	 */
	double getTextBlockHeight (std::vector<std::string> textBlock);


protected:
	virtual void draw (const BUtilities::RectArea& area) override;

	BColors::ColorSet textColors;
	BStyles::Font textFont;
	std::string textString;
	bool yResizable;
};

}

#endif /* BWIDGETS_TEXT_HPP_ */
