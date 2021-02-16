/* PopupListBox.cpp
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

#include "PopupListBox.hpp"

namespace BWidgets
{
PopupListBox::PopupListBox () :
	PopupListBox (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "popuplistbox") {}

PopupListBox::PopupListBox (const double x, const double y, const double width,
			    const double height, const double listWidth,
			    const double listHeight, const std::string& name) :
	PopupListBox (x, y, width, height, 0.0, 0.0, listWidth, listHeight, name, BItems::ItemList (), UNSELECTED) {}

PopupListBox::PopupListBox (const double x, const double y, const double width,
			    const double height, const double listWidth,
			    const double listHeight, const std::string& name,
			    const BItems::ItemList& items, double preselection) :
	PopupListBox (x, y, width, height, 0.0, 0.0, listWidth, listHeight, name, items, preselection) {}

PopupListBox::PopupListBox (const double x, const double y, const double width,
			    const double height, const double listXOffset,
			    const double listYOffset, const double listWidth,
			    const double listHeight, const std::string& name) :
	PopupListBox (x, y, width, height, listXOffset, listYOffset, listWidth, listHeight, name, BItems::ItemList (), UNSELECTED) {}


PopupListBox::PopupListBox (const double x, const double y, const double width,
			    const double height, const double listXOffset,
			    const double listYOffset, const double listWidth,
			    const double listHeight, const std::string& name,
			    const BItems::ItemList& items, double preselection) :
		ItemBox (x, y, width, height, name, {UNSELECTED, nullptr}),
		downButton (0, 0, 0, 0, name + BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_NAME, 0.0),
		listBox (listXOffset, listYOffset, listWidth, listHeight, name + BWIDGETS_DEFAULT_POPUPLISTBOX_LISTBOX_NAME, items, preselection)

{
	setScrollable (true);

	// Set item
	if (preselection != UNSELECTED)
	{
		for (BItems::Item const& i : *listBox.getItemList ())
		{
			if (i.getValue() == preselection)
			{
				value = i.getValue ();
				item.setValue (i.getValue ());
				item.cloneWidgetFrom (i.getWidget ());
				initItem ();
				if (item.getWidget ()) add (*item.getWidget ());
				break;
			}
		}
	}

	downButton.setCallbackFunction (BEvents::EventType::BUTTON_PRESS_EVENT, PopupListBox::handleDownButtonClicked);
	listBox.setCallbackFunction (BEvents::EventType::VALUE_CHANGED_EVENT, PopupListBox::handleValueChanged);
	listBox.setStacking (STACKING_OVERSIZE);
	listBox.hide ();

	add (downButton);
	add (listBox);
}

PopupListBox::PopupListBox (const PopupListBox& that) :
		ItemBox (that), downButton (that.downButton), listBox (that.listBox)
{
	if (item.getWidget ()) add (*item.getWidget ());
	add (downButton);
}

PopupListBox& PopupListBox::operator= (const PopupListBox& that)
{
	downButton = that.downButton;
	listBox = that.listBox;

	ItemBox::operator= (that);
	initItem ();

	return *this;
}

Widget* PopupListBox::clone () const {return new PopupListBox (*this);}

BItems::ItemList* PopupListBox::getItemList () {return listBox.getItemList ();}

ListBox* PopupListBox::getListBox () {return &listBox;}

void PopupListBox::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void PopupListBox::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	ItemBox::applyTheme (theme, name);
	downButton.applyTheme (theme, name + BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_NAME);
	listBox.applyTheme (theme, name + BWIDGETS_DEFAULT_POPUPLISTBOX_LISTBOX_NAME);
}

void PopupListBox::setValue (const double val)
{
	if (val != listBox.getValue ()) listBox.setValue (val);
	if (value != listBox.getValue ())
	{
		// Release old item.widget
		Widget* oldW = item.getWidget ();
		if (oldW && isChild (oldW)) release (oldW);

		// Copy value and clone widget from listbox item
		BItems::Item* listboxItem = listBox.getItem (listBox.getValue ());
		if (listboxItem)
		{
			item.setValue (listboxItem ->getValue ());
			if (listboxItem ->getWidget ()) item.cloneWidgetFrom (listboxItem ->getWidget ());
			initItem ();
		}

		// Add new item.widget and set value
		if (item.getWidget ()) add (*item.getWidget ());
		ValueWidget::setValue (listBox.getValue ());
	}
}

void PopupListBox::moveListBox (const BUtilities::Point& offset)
{
	listBox.moveTo (offset);
}

void PopupListBox::resizeListBox (const BUtilities::Point& extends)
{
	listBox.resize (extends);
}

void PopupListBox::resizeListBoxItem (const double value, const BUtilities::Point& extends)
{
	listBox.resizeItem (value, extends);
}

void PopupListBox::resizeListBoxItems (const BUtilities::Point& extends)
{
	listBox.resizeItems (extends);
}

void PopupListBox::update ()
{
	// Update ItemBox first
	Widget::update ();

	Widget* widget = item.getWidget ();
	if (widget)
	{
		// Set position of label
		double x0 = getXOffset ();
		double y0 = getYOffset ();
		double w = getEffectiveWidth ();
		double dw = (w > BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_WIDTH ? BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_WIDTH : w);
		double w2 = (w - dw > 0 ? w - dw : 0);
		double h = getEffectiveHeight ();

		widget->moveTo (x0, y0);
		widget->resize (w2, h);
	}

	// Keep button on top
	int cs = children_.size ();
	if ((cs >= 2) && (children_[cs - 1] != (Widget*) &downButton))
	{
		downButton.raiseToTop ();
	}

	// Calculate size and position of widget elements
	double x0 = getXOffset ();
	double y0 = getYOffset ();
	double w = getEffectiveWidth ();
	double h = getEffectiveHeight ();

	// Down button
	double dw = (w > BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_WIDTH ? BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_WIDTH : w);
	downButton.moveTo (x0 + w - dw, y0);
	downButton.resize (dw, h);

	// List box
	if (listBox.getPosition() == BUtilities::Point()) listBox.moveTo (BUtilities::Point (0, getHeight()));
}

void PopupListBox::onButtonPressed (BEvents::PointerEvent* event)
{
	if (listBox.isVisible ()) listBox.hide ();
	else
	{
		// Close all other same level popup widgets listboxes first
		Widget* p = getParent();
		if (p)
		{
			for (Widget* c : p->getChildren())
			{
				if (c != this)
				{
					PopupListBox* other = dynamic_cast<PopupListBox*> (c);
					if (other && other->getListBox()) other->getListBox()->hide();
				}
			}
		}

		// Raise to top & show listbox
		raiseToTop ();
		update ();
		listBox.show ();
		listBox.raiseToTop ();
	}

	//Widget::cbfunction[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
}

void PopupListBox::onWheelScrolled (BEvents::WheelEvent* event)
{
	BItems::ItemList* itemList = listBox.getItemList ();
	double newNr = LIMIT (listBox.getActive () - event->getDelta ().y, 1, itemList->size ());
	BItems::ItemList::iterator it = std::next ((*itemList).begin (), newNr - 1);
	setValue ((*it).getValue());
}

void PopupListBox::initItem ()
{
	Widget* w = item.getWidget ();
	if (w)
	{
		w->setClickable (false);
		w->setDraggable (false);
		w->setScrollable (false);
		w->setFocusable (false);
		w->setState (BColors::ACTIVE);
		w->moveTo (0, 0);
		w->show ();
	}
}

void PopupListBox::handleDownButtonClicked (BEvents::Event* event)
{
	if (event && (event->getEventType () == BEvents::EventType::BUTTON_PRESS_EVENT) && event->getWidget ())
	{
		BEvents::PointerEvent* ev = (BEvents::PointerEvent*) event;
		Widget* w = ev->getWidget ();
		if (w->getParent ())
		{
			PopupListBox* p = (PopupListBox*) w->getParent ();
			if (p->getParent ())
			{
				if (w == &(p->downButton)) p->onButtonPressed (ev);
			}
		}
	}
}

void PopupListBox::handleValueChanged (BEvents::Event* event)
{
if (event && (event->getEventType () == BEvents::EventType::VALUE_CHANGED_EVENT) && event->getWidget ())
	{
		BEvents::ValueChangedEvent* ev = (BEvents::ValueChangedEvent*) event;
		ListBox* w = (ListBox*) ev->getWidget ();

		if (w)
		{
			PopupListBox* p = (PopupListBox*) w->getParent();
			if (p)
			{
				p->setValue (w->getValue ());
				p->listBox.hide ();
			}
		}
	}
}

}
