/* PopupListBox.hpp
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

#ifndef BWIDGETS_POPUPLISTBOX_HPP_
#define BWIDGETS_POPUPLISTBOX_HPP_

#include "ItemBox.hpp"
#include "ListBox.hpp"

#define BWIDGETS_DEFAULT_POPUPLISTBOX_WIDTH BWIDGETS_DEFAULT_CHOICEBOX_WIDTH
#define BWIDGETS_DEFAULT_POPUPLISTBOX_HEIGHTH (BWIDGETS_DEFAULT_LISTBOX_HEIGHTH + BWIDGETS_DEFAULT_ITEMBOX_HEIGHT)
#define BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_WIDTH BWIDGETS_DEFAULT_ITEMBOX_HEIGHT
#define BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_HEIGHT BWIDGETS_DEFAULT_ITEMBOX_HEIGHT
#define BWIDGETS_DEFAULT_POPUPLISTBOX_BUTTON_NAME "/button"
#define BWIDGETS_DEFAULT_POPUPLISTBOX_LISTBOX_NAME "/listbox"


namespace BWidgets
{

/**
 * Class BWidgets::PopupListBox
 *
 * Widget for selection of one item out of a vector of items.
 * It is a composite value widget consisting of a the widget itself (a
 * BWidgets::ItemBox) , a BWidgets::ListBox and a BWidgets::DownButton. The
 * widget shows the result of the BWidgets::ListBox. The BWidgets::ListBox pops
 * up on pressing on either the widget itself or BWidgets::DownButton and
 * minimizes again after selection of an item or on pressing on either the
 * widegt or BWidgets::DownButton again. The value of
 * this widget reflects the value the item selected. On
 * change, a value changed event is emitted.
 */
class PopupListBox : public ItemBox
{
public:
	PopupListBox ();
	PopupListBox (const double x, const double y, const double width, const double height,
		      const double listWidth, const double listHeight, const std::string& name);
	PopupListBox (const double x, const double y, const double width, const double height,
		      const double listWidth, const double listHeight, const std::string& name,
		      const BItems::ItemList& items, double preselection = UNSELECTED);
	PopupListBox (const double x, const double y, const double width, const double height,
		      const double listXOffset, const double listYOffset, const double listWidth,
		      const double listHeight, const std::string& name);
	PopupListBox (const double x, const double y, const double width, const double height,
		      const double listXOffset, const double listYOffset, const double listWidth,
		      const double listHeight, const std::string& name,
		      const BItems::ItemList& items, double preselection = UNSELECTED);

	/**
	 * Creates a new (orphan) PopupListBox and copies the properties from a
	 * source choice box widget.
	 * @param that Source choice box
	 */
	PopupListBox (const PopupListBox& that);

	/**
	 * Assignment. Copies the properties from a source PopupListBox widget
	 * and keeps its name and its position within the widget tree. Emits a
	 * BEvents::ExposeEvent if the text widget is visible.
	 * @param that Source text widget
	 */
	PopupListBox& operator= (const PopupListBox& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Gets (a pointer to) the vector of items and thus gets access to the
	 * internally stored list of items.
	 * @return Pointer to a string vector
	 */
	BItems::ItemList* getItemList ();

	/**
	 * Gets (a pointer to) the internal BWidgets::ListBox
	 * @return Pointer to the internal BWidgets::ListBox
	 */
	ListBox* getListBox ();

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

	virtual void moveListBox (const BUtilities::Point& offset);

	virtual void resizeListBox (const BUtilities::Point& extends);

	virtual void resizeListBoxItem (const double value, const BUtilities::Point& extends);

	virtual void resizeListBoxItems (const BUtilities::Point& extends);

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

	/**
	 * Method to handle a click on this widget via a
	 * BEvents::EventType::BUTTON_PRESS_EVENT.
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::WHEEL_SCROLL_EVENT to scroll through the items
	 * in the ItemBox. Changes the widgets value. Scrolling in the ListBox
	 * is NOT handled by this method.
	 * @param event Pointer to a wheel event emitted by the same widget.
	 */
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;

protected:
	void initItem ();
	static void handleValueChanged (BEvents::Event* event);
	static void handleDownButtonClicked (BEvents::Event* event);

	DownButton downButton;
	ListBox listBox;

};

}

#endif /* BWIDGETS_POPUPLISTBOX_HPP_ */
