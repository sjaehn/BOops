/* ItemBox.cpp
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

#include "ItemBox.hpp"

namespace BWidgets
{
ItemBox::ItemBox () :
	ItemBox (0.0, 0.0, 0.0, 0.0, "itembox", BItems::Item {UNSELECTED, nullptr}) {}

ItemBox::ItemBox (const double x, const double y, const double width,
		  const double height, const std::string& name, const BItems::Item item) :
	ValueWidget (x, y, width, height, name, UNSELECTED), item (item)

{
	value = this->item.getValue ();
	background_ = BWIDGETS_DEFAULT_MENU_BACKGROUND;
	border_ = BWIDGETS_DEFAULT_MENU_BORDER;

	Widget* w = this->item.getWidget ();
	if (w)
	{
		w->setClickable (false);
		add (*w);
	}
}

ItemBox::ItemBox (const ItemBox& that) :
	ValueWidget (that), item (that.item)
{
	if (item.getWidget ()) add (*item.getWidget ());
}

ItemBox::~ItemBox () {}

ItemBox& ItemBox::operator= (const ItemBox& that)
{
	setItem (that.item);
	ValueWidget::operator= (that);
	return *this;
}

Widget* ItemBox::clone () const {return new ItemBox (*this);}

void ItemBox::setItem (const BItems::Item item)
{
	bool wasClickable = false;

	// Release old item.widget
	Widget* oldW = item.getWidget ();
	if (oldW && isChild (oldW)) release (oldW);

	// Copy item and set value
	this->item = item;
	setValue (this->item.getValue());

	// Add new item.widget
	Widget* newW = item.getWidget ();
	if (newW)
	{
		newW->setClickable (wasClickable);
		add (*newW);
	}

	update ();
}

BItems::Item* ItemBox::getItem () {return &item;}

void ItemBox::applyTheme (BStyles::Theme& theme) {applyTheme (theme, name_);}

void ItemBox::applyTheme (BStyles::Theme& theme, const std::string& name)
{
	Widget::applyTheme (theme, name);
	if (item.getWidget ())
	{
		item.getWidget ()->applyTheme (theme, name + BWIDGETS_DEFAULT_ITEMBOX_ITEM_NAME);
	}

	update ();
}

void ItemBox::update ()
{
	// Update super widget first
	Widget::update ();

	Widget* widget = item.getWidget ();
	if (widget)
	{
		// Set position of label
		double x0 = getXOffset ();
		double y0 = getYOffset ();
		double w = getEffectiveWidth ();
		double h = getEffectiveHeight ();

		widget->moveTo (x0, y0);
		widget->resize (BUtilities::Point (w, h));
	}
}

}
