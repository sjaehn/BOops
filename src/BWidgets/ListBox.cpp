/* ListBox.cpp
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

#include "ListBox.hpp"

namespace BWidgets
{
ListBox::ListBox () :
	ListBox (0.0, 0.0, 0.0, 0.0, "listbox") {}

ListBox::ListBox (const double x, const double y, const double width, const double height,
	 	  const std::string& name) :
	 ChoiceBox (x, y, width, height, name),
	 listTop (0)
 {
	upButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ListBox::handleButtonClicked);
 	downButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ListBox::handleButtonClicked);
 }

ListBox::ListBox (const double x, const double y, const double width, const double height,
		  const std::string& name, const BItems::ItemList& items, double preselection) :
	ChoiceBox (x, y, width, height, name, items, preselection),
	listTop (0)
{
	if (!items.empty ()) listTop = 1;

	upButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ListBox::handleButtonClicked);
	downButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, ListBox::handleButtonClicked);
}

ListBox::ListBox (const ListBox& that) : ChoiceBox (that), listTop (that.listTop) {}

ListBox::~ListBox () {}

ListBox& ListBox::operator= (const ListBox& that)
{
	listTop = that.listTop;

	ChoiceBox::operator= (that);
	return *this;
}

Widget* ListBox::clone () const {return new ListBox (*this);}

void ListBox::setTop (const int top)
{
	double oldtop = listTop;

	if (items.empty ()) listTop = 0;

	else
	{
		size_t size = items.size ();
		if (top <= 0) listTop = 1;
		else if (((unsigned int) top) <= size) listTop = top;
		else listTop = size;

		int lines = getLines ();
		if (((unsigned int) (getBottom ()) > size) && (size - lines >= 1)) listTop = size - lines + 1;
	}

	if (oldtop != listTop) update ();
}

int ListBox::getTop () const {return listTop;}

void ListBox::onWheelScrolled (BEvents::WheelEvent* event)
{
	setTop (getTop() - event->getDelta ().y);
}

void ListBox::handleButtonClicked (BEvents::Event* event)
{
	if (event && (event->getEventType () == BEvents::EventType::BUTTON_PRESS_EVENT) && event->getWidget ())
	{
		BEvents::PointerEvent* ev = (BEvents::PointerEvent*) event;
		Button* w = (Button*) ev->getWidget ();
		if (w->getParent ())
		{
			ListBox* p = (ListBox*) w->getParent ();
			if (p->getParent ())
			{
				if (w == (Button*) &(p->upButton)) p->setTop (p->getTop () - 1);
				if (w == (Button*) &(p->downButton)) p->setTop (p->getTop () + 1);
			}
		}
	}
}

void ListBox::updateItems ()
{
	double x0 = getXOffset ();
	double y0 = getYOffset ();
	double width = getEffectiveWidth ();
	double height = getEffectiveHeight ();
	double listHeight = (height >= 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
			     height - 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : 0);
	double upButtonHeight = (height >= BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
				 BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : 0); // TODO
	double itemHeight = (!items.empty () && items.front ().getWidget () ?
			     items.front ().getWidget()->getHeight () :
			     BWIDGETS_DEFAULT_CHOICEBOX_ITEM_HEIGHTH);
	double lineHeight = ((items.empty () || (itemHeight == 0)) ? 20 : itemHeight);
	unsigned int lines = ceil (listHeight / lineHeight);

	unsigned int n = 0;
	for (BItems::Item const& i : items)
	{
		Widget* w = i.getWidget ();
		if (w)
		{
			if ((n + 1 >= ((unsigned int) listTop)) && (n + 1 < listTop + lines))
			{
				w->moveTo (x0, y0 + upButtonHeight + (n + 1 - listTop) * lineHeight);
				w->resize (width, lineHeight);

				if (n + 1 == ((unsigned int) (activeNr))) w->setState (BColors::ACTIVE);
				else w->setState (BColors::NORMAL);

				w->show ();
			}

			else w->hide ();
		}
		++n;
	}
}

int ListBox::getLines ()
{
	double height = getEffectiveHeight ();
	double listHeight = (height >= 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT ?
			     height - 2 * BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT : 0);
	double lineHeight = (((!items.empty ()) && items.front ().getWidget ()) ?
			     items.front ().getWidget()->getHeight () :
			     BWIDGETS_DEFAULT_CHOICEBOX_ITEM_HEIGHTH);
	int lines = (listHeight > lineHeight ? listHeight / lineHeight : 1);
	return lines;
}

}
