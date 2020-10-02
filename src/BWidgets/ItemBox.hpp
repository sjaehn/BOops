/* ItemBox.hpp
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

#ifndef BWIDGETS_ITEMBOX_HPP_
#define BWIDGETS_ITEMBOX_HPP_

#include "BItems.hpp"
#include "Label.hpp"
#include "ValueWidget.hpp"
#include <cmath>

#define BWIDGETS_DEFAULT_ITEMBOX_WIDTH 100.0
#define BWIDGETS_DEFAULT_ITEMBOX_HEIGHT 20.0

#define BWIDGETS_DEFAULT_ITEMBOX_ITEM_NAME "/item"

#ifndef UNSELECTED
#define UNSELECTED -HUGE_VAL
#endif

namespace BWidgets
{

/**
 * Class BWidgets::ItemBox
 *
 * Single line box widget displaying an item widget. An item is a value
 * associated with a widget. If no widget has been selected (nullptr), an
 * internal label widget will be used.
 *
 * TODO: Flexible padding
 */
class ItemBox : public ValueWidget
{
public:
	ItemBox ();
	ItemBox (const double x, const double y, const double width, const double height,
		 const std::string& name, const BItems::Item item);

	/**
	 * Creates a new (orphan) item box and copies the properties from a
	 * source item box widget.
	 * @param that Source choice box
	 */
	ItemBox (const ItemBox& that);

	~ItemBox ();

	/**
	 * Assignment. Copies the properties from a source item box widget
	 * and keeps its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the text widget is visible.
	 * @param that Source text widget
	 */
	ItemBox& operator= (const ItemBox& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Sets the item stored in this widget.
	 * @param item	Item.
	 */
	void setItem (const BItems::Item item);

	/**
	 * Gets (a pointer to) the item stored in this widget.
	 * @return	Item.
	 */
	BItems::Item* getItem ();

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

protected:
	BItems::Item item;
};

}

#endif /* BWIDGETS_ITEMBOX_HPP_ */
