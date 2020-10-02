/* MessageBox.cpp
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

#include "MessageBox.hpp"

namespace BWidgets
{
MessageBox::MessageBox () : MessageBox (0.0, 0.0, 0.0, 0.0, "messagebox", "", "MessageBox") {}

MessageBox::MessageBox (const double x, const double y, const double width, const double height,
			const std::string& text, std::vector<std::string> buttons) :
		MessageBox (x, y, width, height, text, "", text, buttons) {}

MessageBox::MessageBox (const double x, const double y, const double width, const double height,
			const std::string& title, const std::string& text, std::vector<std::string> buttons) :
		MessageBox (x, y, width, height, title, title, text, buttons) {}

MessageBox::MessageBox (const double x, const double y, const double width, const double height,
			const std::string& name, const std::string& title, const std::string& text, std::vector<std::string> buttonLabels) :
		ValueWidget (x, y, width, height, name, 0.0),
		titleBox (0, 0, 0, 0, name + BWIDGETS_DEFAULT_MESSAGEBOX_TITLE_NAME, ""),
		textBox (0, 0, 0, 0, name + BWIDGETS_DEFAULT_MESSAGEBOX_TEXT_NAME, ""),
		okButton (0, 0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT, name + BWIDGETS_DEFAULT_MESSAGEBOX_BUTTON_NAME, "OK", 0.0),
		buttons ()
{
	// Title
	setTitle (title);
	titleBox.setClickable (false);
	add (titleBox);

	// Text
	setText (text);
	textBox.setClickable (false);
	add (textBox);

	// Buttons
	if (!buttonLabels. empty ()) addButtons (buttonLabels);

	// Or at least the OK button
	if (buttons.empty ())
	{
		okButton.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, MessageBox::redirectPostValueChanged);
		buttons.push_back (&okButton);
		add (okButton);
	}

	background_ = BWIDGETS_DEFAULT_MENU_BACKGROUND;
	border_ = BWIDGETS_DEFAULT_MENU_BORDER;
	setDraggable (true);
}

MessageBox::MessageBox (const MessageBox& that) : ValueWidget (that)
{
	titleBox = that.titleBox;
	add (titleBox);
	textBox = that.textBox;
	add (textBox);

	for (TextButton* b : that.buttons)
	{
		addButton (b->getLabel ()->getText ());
	}

	okButton = that.okButton;
	if (buttons.empty ())
	{
		okButton.getLabel ()->setTextColors(BColors::darks);
		okButton.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, MessageBox::redirectPostValueChanged);
		buttons.push_back (&okButton);
		add (okButton);
	}

	update ();
}

MessageBox::~MessageBox ()
{
	while (!buttons.empty ())
	{
		TextButton* b = buttons.back ();
		if (b && (b != &okButton)) delete b;
		else release (b);
		buttons.pop_back ();
	}
}

MessageBox& MessageBox::operator= (const MessageBox& that)
{
	titleBox = that.titleBox;
	textBox = that.textBox;
	okButton = that.okButton;

	// Clean buttons first
	while (!buttons.empty ())
	{
		TextButton* b = buttons.back ();
		if (b && (b != &okButton)) delete b;
		else release (b);
		buttons.pop_back ();
	}

	// Hard copy buttons
	for (TextButton* b : that.buttons)
	{
		addButton (b->getLabel ()->getText ());
	}

	// At least an OK button
	if (buttons.empty ())
	{
		okButton.getLabel ()->setTextColors(BColors::darks);
		okButton.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, MessageBox::redirectPostValueChanged);
		buttons.push_back (&okButton);
		add (okButton);
	}

	Widget::operator= (that);
	return *this;
}

Widget* MessageBox::clone () const {return new MessageBox (*this);}

void MessageBox::setTitle (const std::string& title) {titleBox.setText (title);}

std::string MessageBox::getTitle () const {return titleBox.getText ();}

void MessageBox::setText (const std::string& text) {textBox.setText (text);}

std::string MessageBox::getText () const {return textBox.getText ();}

void MessageBox::addButton (const std::string& label)
{
	TextButton* b = new TextButton(0, 0, BWIDGETS_DEFAULT_BUTTON_WIDTH, BWIDGETS_DEFAULT_BUTTON_HEIGHT,
					name_ + BWIDGETS_DEFAULT_MESSAGEBOX_BUTTON_NAME, label, 0.0);
	if (b)
	{
		cairo_t* cr = cairo_create (widgetSurface_);
		cairo_text_extents_t ext = b->getLabel()->getFont()->getTextExtents (cr, label);
		cairo_destroy (cr);
		b->setWidth (ext.width > BWIDGETS_DEFAULT_BUTTON_WIDTH - BWIDGETS_DEFAULT_MENU_PADDING ?
				ext.width + BWIDGETS_DEFAULT_MENU_PADDING :
				BWIDGETS_DEFAULT_BUTTON_WIDTH);
		b->setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, MessageBox::redirectPostValueChanged);
		buttons.push_back (b);
		add (*b);
	}
}

void MessageBox::addButtons (std::vector<std::string> labels)
{
	for (std::string label : labels) addButton (label);
}

void MessageBox::removeButton  (const std::string& label)
{
	for (std::vector<TextButton*>::iterator it = buttons.begin (); it != buttons.end (); ++it)
	{
		TextButton* b = (TextButton*) *it;
		if (b && (b->getLabel ()->getText () == label))
		{
			if (b != &okButton) delete b;
			buttons.erase (it);
			return;
		}
	}
}

double MessageBox::getButtonValue  (const std::string& label) const
{
	double nr = 1;
	for (TextButton* b : buttons)
	{
		if (b)
		{
			if (b->getLabel ()->getText () == label) return nr;
			nr++;
		}
	}
	return 0;
}

std::string MessageBox::getButtonText (const double value) const
{
	size_t v = value;
	if ((v < 1) || (v > buttons.size ())) return "";
	if (!buttons[v - 1]) return "";
	return buttons[v - 1]->getLabel ()-> getText ();
}

void MessageBox::setTextColors (const BColors::ColorSet& colorset) {textBox.setTextColors (colorset);}

BColors::ColorSet* MessageBox::getTextColors () {return textBox.getTextColors ();}

void MessageBox::setFont (const BStyles::Font& font) {textBox.setFont (font);}

BStyles::Font* MessageBox::getFont () {return textBox.getFont ();}

void MessageBox::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void MessageBox::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);
	titleBox.applyTheme (theme, name + BWIDGETS_DEFAULT_MESSAGEBOX_TITLE_NAME);
	textBox.applyTheme (theme, name + BWIDGETS_DEFAULT_MESSAGEBOX_TEXT_NAME);
	for (TextButton* b : buttons)
	{
		if (b) b->applyTheme (theme, name + BWIDGETS_DEFAULT_MESSAGEBOX_BUTTON_NAME);
	}
	update ();
}

void MessageBox::update ()
{
	// Update super widget first
	Widget::update ();

	double width = getEffectiveWidth ();
	double height = getEffectiveHeight ();

	// Title
	if (titleBox.getText () != "")
	{
		titleBox.setWidth (width > 2 * BWIDGETS_DEFAULT_MENU_PADDING ? width - 2 * BWIDGETS_DEFAULT_MENU_PADDING : 0);
		titleBox.setHeight
		(
			height > 3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT ?
			height - (3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) :
			0
		);
		titleBox.getFont ()->setFontWeight (CAIRO_FONT_WEIGHT_BOLD);
		double titleheight = titleBox.getTextBlockHeight(titleBox.getTextBlock ());
		titleBox.setHeight
		(
			titleheight < height - (3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) ?
			titleheight :
			(
				height > (3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) ?
				height - (3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) :
				0
			)
		);
		titleBox.moveTo (BWIDGETS_DEFAULT_MENU_PADDING, BWIDGETS_DEFAULT_MENU_PADDING);
	}
	else
	{
		titleBox.setWidth (0);
		titleBox.setHeight (0);
		titleBox.moveTo (0, 0);
	}

	// Text
	if (textBox.getText () != "")
	{
		double titleheight = titleBox.getHeight () + BWIDGETS_DEFAULT_MENU_PADDING;
		textBox.setWidth (width > 2 * BWIDGETS_DEFAULT_MENU_PADDING ? width - 2 * BWIDGETS_DEFAULT_MENU_PADDING : 0);
		textBox.setHeight
		(
			height > (3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) + titleheight ?
			height - ((3 * BWIDGETS_DEFAULT_MENU_PADDING + BWIDGETS_DEFAULT_BUTTON_HEIGHT) + titleheight) :
			0
		);
		textBox.moveTo (BWIDGETS_DEFAULT_MENU_PADDING, BWIDGETS_DEFAULT_MENU_PADDING + titleheight);
	}
	else
	{
		textBox.setWidth (0);
		textBox.setHeight (0);
		textBox.moveTo (0, 0);
	}

	// Buttons
	// Calculate total width
	int nrbuttons = 0;
	double totalbuttonwidth = 0.0;
	for (TextButton* b : buttons)
	{
		if (b)
		{
			++nrbuttons;
			totalbuttonwidth += b->getWidth ();
		}
	}

	// Calculate spaces and offset
	double buttonspace = (width - totalbuttonwidth) / (nrbuttons + 1);
	if (buttonspace < BWIDGETS_DEFAULT_MENU_PADDING) buttonspace = BWIDGETS_DEFAULT_MENU_PADDING;
	double buttonxpos = (totalbuttonwidth + (nrbuttons + 1) * buttonspace < width ?
			     buttonspace :
			     width / 2 - (totalbuttonwidth + (nrbuttons - 1) * buttonspace) / 2);

	// Rearrange
	for (TextButton* b : buttons)
	{
		if (b)
		{
			b->moveTo (buttonxpos, height - BWIDGETS_DEFAULT_MENU_PADDING - BWIDGETS_DEFAULT_BUTTON_HEIGHT);
			buttonxpos = buttonxpos + buttonspace + b->getWidth ();
		}
	}
}

void MessageBox::redirectPostValueChanged (BEvents::Event* event)
{
	if (event && (event->getEventType () == BEvents::EventType::VALUE_CHANGED_EVENT) && event->getWidget ())
	{
		BEvents::ValueChangedEvent* ev = (BEvents::ValueChangedEvent*) event;
		TextButton* w = (TextButton*) ev->getWidget ();
		if (w->getParent ())
		{
			std::string label = w->getLabel ()->getText ();
			MessageBox* p = (MessageBox*) w->getParent ();
			if (p->getMainWindow ())
			{
				// Button pressed ?
				if (w->getValue ())
				{
					double v = p->getButtonValue (label);

					// Emit value changed event and close (hide) message box
					if (v)
					{
						p->setValue (v);
						p->postCloseRequest ();
					}
				}
			}
		}
	}
}

}
