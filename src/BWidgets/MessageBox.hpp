/* MessageBox.hpp
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

#ifndef BWIDGETS_MESSAGEBOX_HPP_
#define BWIDGETS_MESSAGEBOX_HPP_

#include "ValueWidget.hpp"
#include "Text.hpp"
#include "TextButton.hpp"

#define BWIDGETS_DEFAULT_MESSAGEBOX_TITLE_NAME "/title"
#define BWIDGETS_DEFAULT_MESSAGEBOX_TEXT_NAME "/text"
//TODO #define BWIDGETS_DEFAULT_MESSAGEBOX_ICON_NAME "/icon"
#define BWIDGETS_DEFAULT_MESSAGEBOX_BUTTON_NAME "/button"

namespace BWidgets
{

/**
 * Class BWidgets::MessageBox
 *
 * Message box. It is a composite value widget consisting of an optional title,
 * a Text, (TODO an optional icon) and an OK button. The widget is dragable.
 * The value of this widget is 0.0 until one of the buttons is pressed. At this
 * moment, the value reflects the number of the button starting with 1.0. Also
 * a value changed event is emitted and the message box is closed (released) on
 * button pressed.
 */
class MessageBox : public ValueWidget
{
public:
	MessageBox ();
	MessageBox (const double x, const double y, const double width, const double height,
				const std::string& text, std::vector<std::string> buttons = {});
	MessageBox (const double x, const double y, const double width, const double height,
				const std::string& title, const std::string& text, std::vector<std::string> buttons = {});
	MessageBox (const double x, const double y, const double width, const double height,
				const std::string& name, const std::string& title, const std::string& text, std::vector<std::string> buttonLabels = {});

	/**
	 * Creates a new (orphan) message box and copies the properties from a
	 * source message box widget.
	 * @param that Source message box
	 */
	MessageBox (const MessageBox& that);

	~MessageBox ();

	/**
	 * Assignment. Copies the properties from a source message box widget
	 *  and keeps its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the text widget is visible.
	 * @param that Source text widget
	 */
	MessageBox& operator= (const MessageBox& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	//TODO virtual void setWidth (const double width) override;

	//TODO virtual void setHeight (const double height) override;

	/**
	 * Sets the title.
	 * @param title Title
	 */
	void setTitle (const std::string& title);

	/**
	 * Gets the title
	 * @return Title
	 */
	std::string getTitle () const;

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
	 * Creates a button and adds it to the message box from left to right.
	 * @param label Label text string of the new button
	 */
	void addButton (const std::string& label);

	/**
	 * Creates and adds a series of buttons to the message box from left to
	 * right.
	 * @param labels Vector of label text strings of the new buttons
	 */
	void addButtons (std::vector<std::string> labels);

	/**
	 * Removes and deletes a button. Exception: the internal OK button can
	 * only be removed. Removing may cause renumbering of the buttons.
	 * @param label Label text of the button to be removed. If there is more
	 * 				than one button with this label text, only the first one
	 * 				will be removed
	 */
	void removeButton (const std::string& label);

	/**
	 * Gets the number of the button as in the order of addition (= order from
	 * left to right. This is also the value that is be emitted if this button
	 * is pressed.
	 * @param label	Label text string of the button
	 * @return		Number of the first button with the same label text string
	 * 				as @param label. Starting from 1.
	 * 				0 is returned, if there is no button with this label text.
	 */
	double getButtonValue  (const std::string& label) const;

	std::string getButtonText (const double value) const;

	//TODO virtual void setBorder (const BStyles::Border& border) override;

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
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

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

protected:
	static void redirectPostValueChanged (BEvents::Event* event);

	Text titleBox;
	Text textBox;
	// TODO Icon;
	TextButton okButton;
	std::vector<TextButton*> buttons;
};

}

#endif /* BWIDGETS_MESSAGEBOX_HPP_ */
