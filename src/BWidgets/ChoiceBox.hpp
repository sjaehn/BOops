/* ChoiceBox.hpp
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

#ifndef BWIDGETS_CHOICEBOX_HPP_
#define BWIDGETS_CHOICEBOX_HPP_

#include "ValueWidget.hpp"
#include "UpButton.hpp"
#include "DownButton.hpp"
#include "BItems.hpp"
#include <cmath>

#define BWIDGETS_DEFAULT_CHOICEBOX_WIDTH 100.0
#define BWIDGETS_DEFAULT_CHOICEBOX_HEIGHTH 40.0
#define BWIDGETS_DEFAULT_CHOICEBOX_ITEM_HEIGHTH 20.0
#define BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_HEIGHT 9.0
#define BWIDGETS_DEFAULT_CHOICEBOX_LINE_HEIGHT 1.6666

#define BWIDGETS_DEFAULT_CHOICEBOX_BUTTON_NAME "/button"
#define BWIDGETS_DEFAULT_CHOICEBOX_ITEM_NAME "/item"

#ifndef UNSELECTED
#define UNSELECTED -HUGE_VAL
#endif

namespace BWidgets
{

/**
 * Class BWidgets::ChoiceBox
 *
 * Base Widget for selection of one item out of a vector of items.
 * It is a composite value widget consisting of a vector of items, an up
 * button, and a down button. Selection directly results using the buttons.
 * The widget only shows the selected item. The value of this widget reflects
 * the number the item selected starting with 1.0 (default). On change, a value
 * changed event is emitted and this widget.
 */
class ChoiceBox : public ValueWidget
{
public:
	ChoiceBox ();
	ChoiceBox (const double x, const double y, const double width, const double height,
		   const std::string& name);
	ChoiceBox (const double x, const double y, const double width, const double height,
		   const std::string& name, const BItems::ItemList& items, double preselection = UNSELECTED);

	/**
	 * Creates a new (orphan) choice box and copies the properties from a
	 * source choice box widget.
	 * @param that Source choice box
	 */
	ChoiceBox (const ChoiceBox& that);

	~ChoiceBox ();

	/**
	 * Assignment. Copies the properties from a source choice box widget
	 * and keeps its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the text widget is visible.
	 * @param that Source text widget
	 */
	ChoiceBox& operator= (const ChoiceBox& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Gets (a pointer to) the vector of items and thus gets access to the
	 * internally stored list of items.
	 * @return Pointer to a BItems::ItemList
	 */
	BItems::ItemList* getItemList ();

	/**
	 * Gets (a pointer to) the item of the internally stored list of items
	 * that is represented by a value.
	 * @param value	Value of the item.
	 * @return	Pointer to the item.
	 */
	BItems::Item* getItem (const double value);

	/**
	 * Gets (a pointer to) the active item of the internally stored list.
	 * @return	Pointer to the active item
	 */
	BItems::Item* getActiveItem ();

	void removeItems ();

	/**
	 * Adds a new item or new items to the end of the internally stored list of
	 * items.
	 * @param newBItems::Item	A single new item to add.
	 * @param newBItems::Items	A vector of new items to add.
	 */
	void addItem (const BItems::Item& newItem);
	void addItem (const BItems::ItemList& newItems);

	/**
	 * Resizes a single items widget.
	 * @param value		Value of the item.
	 * @param width 	New width of the item.
	 * @param height	New height of the item.
	 */
	virtual void resizeItem (const double value, const BUtilities::Point& extends);

	/**
	 * Resizes a all items widgets.
	 * @param width 	New width of the items.
	 * @param height	New height of the items.
	 */
	virtual void resizeItems (const BUtilities::Point& extends);

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Changes the value of the widget. Emits a value changed event and (if
	 * visible) an expose event.
	 * @param val Value
	 */
	virtual void setValue (const double val) override;

	/**
	 * Gets the number of top line of the shown list. In this case
	 * (BWidgets::ChoiceBox) it returns the number (not the value!) of the
	 * active item.
	 * @param return Top line of the list (starting with 1.0)
	 */
	virtual int getTop () const;

	/**
	 * Gets the number (NOT the value!) of the active line of the shown list.
	 * @param return Active line of the list (starting with 1.0)
	 */
	virtual int getActive () const;

	/**
	 * Gets the bottom line of the shown list.
	 * @param return Bottom line of the list (Top starting with 1.0)
	 */
	int getBottom ();

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

	/**
	 * Handles the BEvents::WHEEL_SCROLL_EVENT to scroll through the items.
	 * Changes the widgets value.
	 * @param event Pointer to a wheel event emitted by the same widget.
	 */
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;

protected:
	virtual void updateItems ();
	virtual int getLines ();
	static void handleButtonClicked (BEvents::Event* event);
	static void handleItemClicked (BEvents::Event* event);

	UpButton upButton;
	DownButton downButton;
	BItems::ItemList items;

	int activeNr;
};

}

#endif /* BWIDGETS_CHOICEBOX_HPP_ */
