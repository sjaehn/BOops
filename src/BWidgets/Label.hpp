/* Label.hpp
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

#ifndef BWIDGETS_LABEL_HPP_
#define BWIDGETS_LABEL_HPP_

#define BWIDGETS_DEFAULT_LABEL_WIDTH 60
#define BWIDGETS_DEFAULT_LABEL_HEIGHT 20
#define BWIDGETS_DEFAULT_LABEL_ALIGN BStyles::TEXT_ALIGN_CENTER
#define BWIDGETS_DEFAULT_LABEL_VALIGN BStyles::TEXT_VALIGN_MIDDLE

#define BWIDGETS_LABEL_TEXT_CHANGED_MESSAGE "TEXT_CHANGED"
#define BWIDGETS_LABEL_EDIT_ENTERED_MESSAGE "EDIT_ENTERED"

#include "Widget.hpp"

namespace BWidgets
{

enum LabelPosition
{
	LABEL_CENTER,
	LABEL_TOP,
	LABEL_BOTTOM,
	LABEL_RIGHT,
	LABEL_LEFT
};

/**
 * Class BWidgets::Label
 *
 * Simple text output widget.
 */
class Label : public Widget
{
public:
	Label ();
	Label (const double x, const double y, const double width, const double height, const std::string& text);
	Label (const double x, const double y, const double width, const double height, const std::string& name, const std::string& text);

	/**
	 * Assignment. Copies the label properties from a source label and keeps
	 * its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the label is visible.
	 * @param that Source label
	 */
	Label& operator= (const Label& that);

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
	 * Gets the effective width of a text with the given font
	 * @param text		Text string
	 * @param return	Effective width of the text
	 */
	double getTextWidth (std::string& text);

	/**
	 * Resizes the widget, redraw and emits a BEvents::ExposeEvent if the
	 * widget is visible. If no parameters are given, the widget will be
	 * resized to the size of the containing child widgets or to the text
	 * extends (what is higher).
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

	void setEditable (const bool status);
	bool isEditable () const;

	void setEditMode (const bool mode);
	bool getEditMode () const;

	void setCursor (const size_t pos);
	void setCursor (const size_t from, const size_t to);

	void applyEdit ();
	void discardEdit ();

	virtual void onButtonClicked (BEvents::PointerEvent* event) override;
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;
	virtual void onKeyPressed (BEvents::KeyEvent* event) override;
	virtual void onKeyReleased (BEvents::KeyEvent* event) override;

protected:
	size_t getCursorFromCoords (const BUtilities::Point& position);
	virtual void draw (const BUtilities::RectArea& area) override;

	BColors::ColorSet labelColors;
	BStyles::Font labelFont;
	std::string labelText;
	std::string oldText;
	std::u32string u32labelText;
	bool editable;
	bool editMode;
	size_t cursorFrom;
	size_t cursorTo;
};

}

#endif /* BWIDGETS_LABEL_HPP_ */
