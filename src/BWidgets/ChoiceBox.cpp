/* ChoiceBox.cpp
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

#include "ChoiceBox.hpp"

namespace BWidgets
{
ChoiceBox::ChoiceBox () : ChoiceBox (0.0, 0.0, 0.0, 0.0, "choicebox") {}

ChoiceBox::ChoiceBox (const double x, const double y, const double width,
		      const double height, const std::string& name) :
	ValueWidget (x, y, width, height, name, UNSELECTED),
	upButton (0, 0, 0, 0, name + BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_NAME, 0.0),
	downButton (0, 0, 0, 0, name + BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_NAME, 0.0),
	items (), activeNr (0)
{
	background_ = BWIDGETS_DEFAULT_MENU_BACKGROUND;
	border_ = BWIDGETS_DEFAULT_MENU_BORDER;

	upButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ChoiceBox::handleButtonClicked);
	downButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ChoiceBox::handleButtonClicked);

	add (upButton);
	add (downButton);
}

ChoiceBox::ChoiceBox (const double x, const double y, const double width, const double height,
		      const std::string& name, const BItems::ItemList& items, double preselection) :
	ChoiceBox (x, y, width, height, name)

{
	addItem (items);

	// Set value and preselection
	value = preselection;

	unsigned int n = 0;
	for (BItems::Item const& i : items)
	{
		if (preselection == i.getValue())
		{
			activeNr = n + 1;
			break;
		}
		++n;
	}
}

ChoiceBox::ChoiceBox (const ChoiceBox& that) :
	ValueWidget (that), upButton (that.upButton), downButton (that.downButton),
	items (), activeNr (that.activeNr)
{
	addItem (that.items);
	add (upButton);
	add (downButton);
}

ChoiceBox::~ChoiceBox () {}

ChoiceBox& ChoiceBox::operator= (const ChoiceBox& that)
{
	removeItems ();
	addItem (that.items);

	upButton = that.upButton;
	downButton = that.downButton;

	activeNr = that.activeNr;

	ValueWidget::operator= (that);
	return *this;
}

Widget* ChoiceBox::clone () const {return new ChoiceBox (*this);}

void ChoiceBox::removeItems ()
{
	while (!items.empty ())
	{
		if (items.back().getWidget ()) release (items.back().getWidget ());
		items.pop_back ();
	}
}

BItems::ItemList* ChoiceBox::getItemList () {return &items;}

BItems::Item* ChoiceBox::getItem (const double val)
{
	return items.getItem (val);
}

BItems::Item* ChoiceBox::getActiveItem ()
{
	if ((activeNr >= 1) && (activeNr <= int (items.size ())))
	{
		BItems::ItemList::iterator it = std::next (items.begin (), activeNr - 1);
		return &(*it);
	}
	else return nullptr;
}

void ChoiceBox::addItem (const BItems::Item& newItem)
{
	items.push_back (newItem);
	Widget* w = items.back ().getWidget ();
	if (w)
	{
		w->setClickable (true);
		w->setScrollable (false);
		w->setCallbackFunction (BEvents::BUTTON_PRESS_EVENT, ChoiceBox::handleItemClicked);
		add (*w);
	}
	if (isVisible ()) update ();
}

void ChoiceBox::addItem (const BItems::ItemList& newItems)
{
	for (BItems::Item const& ni : newItems) addItem (ni);
}

void ChoiceBox::resizeItem (const double value, const BUtilities::Point& extends)
{
	BItems::Item* it = getItem (value);
	if (it)
	{
		BWidgets::Widget* w = it->getWidget ();
		if (w) w->resize (extends);
	}

	updateItems ();
}

void ChoiceBox::resizeItems (const BUtilities::Point& extends)
{
	for (BItems::Item const& it : items)
	{
		BWidgets::Widget* w = it.getWidget ();
		if (w) w->resize (extends);
	}

	updateItems ();
}

void ChoiceBox::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void ChoiceBox::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);
	for (BItems::Item const& i : items)
	{
		if (i.getWidget ())
		{
			i.getWidget()->applyTheme (theme, name + BWIDGETS_DEFAULT_CHOICEBOX_ITEM_NAME);
		}
	}
	upButton.applyTheme (theme, name + BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_NAME);
	downButton.applyTheme (theme, name + BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_NAME);
	update ();
}

void ChoiceBox::setValue (const double val)
{
	if (items.empty () || (val == UNSELECTED))
	{
		ValueWidget::setValue (UNSELECTED);
		activeNr = 0;
	}

	else
	{
		unsigned int n = 0;
		for (BItems::Item const& i : items)
		{
			if (val == i.getValue ())
			{
				ValueWidget::setValue (val);
				activeNr = n + 1;
				return;
			}
			++n;
		}

		ValueWidget::setValue (UNSELECTED);
		activeNr = 0;
	}
}

int ChoiceBox::getTop () const {return activeNr;}

int ChoiceBox::getActive () const {return activeNr;}

int ChoiceBox::getBottom () {return (getTop () + getLines () - 1);}

int ChoiceBox::getLines () {return 1.0;}

void ChoiceBox::update ()
{
	// Update super widget first
	Widget::update ();

	// Validate value and update activeNr
	setValue (getValue ());

	// Keep Buttons on top
	int cs = children_.size ();
	if ((cs >= 2) &&
	    ((children_[cs - 1] != (Widget*) &upButton) ||
	     (children_[cs - 2] != (Widget*) &downButton)))
	{
		downButton.raiseToTop ();
		upButton.raiseToTop ();
	}

	// Update items
	updateItems ();

	// Set position of buttons and item label
	double x0 = getXOffset ();
	double y0 = getYOffset ();
	double width = getEffectiveWidth ();
	double height = getEffectiveHeight ();

	if (getTop () > 1.0) upButton.show ();
	else upButton.hide ();
	double upButtonHeight = (height >= BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
				 BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : height);
	upButton.moveTo (x0, y0);
	upButton.resize (width, upButtonHeight);

	if (height > BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT)
	{
		if (getBottom () < int (items.size ())) downButton.show ();
		else downButton.hide ();
		double downButtonHeight = (height >= 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
					   BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT :
					   height - BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT);
		downButton.moveTo (x0, y0 + height - downButtonHeight);
		downButton.resize (width, downButtonHeight);
	}
}

void ChoiceBox::updateItems ()
{
	double x0 = getXOffset ();
	double y0 = getYOffset ();
	double width = getEffectiveWidth ();
	double height = getEffectiveHeight ();
	double itemHeight = (height >= 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
			     height - 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : 0);
	double upButtonHeight = (height >= BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
				 BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : 0); // TODO

	unsigned int n = 0;
	for (BItems::Item const& i : items)
	{
		Widget* w = i.getWidget ();
		if (w)
		{
			if (n + 1 == ((unsigned int) activeNr)) w->show ();
			else w->hide ();

			w->moveTo (x0, y0 + upButtonHeight);
			w->resize (width, itemHeight);
		}
		++n;
	}
}

void ChoiceBox::onWheelScrolled (BEvents::WheelEvent* event)
{
	double newNr = LIMIT (activeNr - event->getDelta ().y, 1, items.size ());
	BItems::ItemList::iterator it = std::next (items.begin (), newNr - 1);
	setValue ((*it).getValue ());
}

void ChoiceBox::handleButtonClicked (BEvents::Event* event)
{
	if (event && (event->getEventType () == BEvents::EventType::BUTTON_PRESS_EVENT) && event->getWidget ())
	{
		BEvents::PointerEvent* ev = (BEvents::PointerEvent*) event;
		Button* w = (Button*) ev->getWidget ();
		if (w->getParent ())
		{
			ChoiceBox* p = (ChoiceBox*) w->getParent ();
			if (p->getParent ())
			{
				double actNr = p->activeNr;
				if ((w == (Button*) &(p->upButton)) && (actNr >= 2))
				{
					BItems::ItemList::iterator it = std::next (p->items.begin(), actNr - 2);
					p->setValue ((*it).getValue());
				}
				else if ((w == (Button*) &(p->downButton)) && (actNr < p->items.size ()))
				{
					BItems::ItemList::iterator it = std::next (p->items.begin(), actNr);
					p->setValue ((*it).getValue());
				}
			}
		}
	}
}

void ChoiceBox::handleItemClicked (BEvents::Event* event)
{
	if (!event) return;
	if (event->getEventType () != BEvents::EventType::BUTTON_PRESS_EVENT) return;
	BEvents::PointerEvent* ev = (BEvents::PointerEvent*) event;
	Widget* w = ev->getWidget ();
	if (!w) return;
	ChoiceBox* p = (ChoiceBox*) w->getParent ();
	if (!p) return;

	for (BItems::Item const& i : p->items)
	{
		if (w == i.getWidget())
		{
			p->setValue (i.getValue());
			break;
		}
	}
}

}
