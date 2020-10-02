/* BItems.hpp
 * Copyright (C) 2019  Sven Jähnichen
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

 #ifndef BWIDGETS_BITEMS_HPP_
 #define BWIDGETS_BITEMS_HPP_

#include <list>
#include <initializer_list>
#include "Widget.hpp"
#include <cmath>

#ifndef UNSELECTED
#define UNSELECTED -HUGE_VAL
#define BITEMS_DEFAULT_TEXT_PADDING 4.0
#endif

namespace BItems
{

class Item
{
public:
        Item ();
        Item (const double value, BWidgets::Widget* widget);
        Item (const double value, const std::string& text);
        Item (const Item& that);

        ~Item ();

        Item& operator= (const Item& that);

        void setValue (const double value);
        double getValue () const;
        void setWidget (BWidgets::Widget* widget);
        void setWidget (const std::string& text);
        void cloneWidgetFrom (BWidgets::Widget* widget);
        BWidgets::Widget* getWidget () const;

protected:
        void deleteInternal ();

        double value;
        BWidgets::Widget* widget;
        BWidgets::Widget* internal;
};

class ItemList : private std::list<Item>
{
public:
        ItemList ();
        ItemList (const Item& item);
        ItemList (const std::list<Item>& items);
        ItemList (BWidgets::Widget* widget);
        ItemList (const std::initializer_list<BWidgets::Widget*>& widgets);
        ItemList (const std::string& text);
        ItemList (const std::initializer_list<std::string>& texts);

        using std::list<Item>::operator=;
        using std::list<Item>::begin;
        using std::list<Item>::end;
        using std::list<Item>::size;
        using std::list<Item>::empty;
        using std::list<Item>::front;
        using std::list<Item>::back;
        using std::list<Item>::pop_back;
        using std::list<Item>::pop_front;
        using std::list<Item>::erase;
        using std::list<Item>::iterator;

        void push_back (const Item& item);
        void push_back (BWidgets::Widget* widget);
        void push_back (const std::string& text);
        // TODO void push_front
        // TODO iterator insert

        Item* getItem (const double value);
        // TODO renumber

protected:
        double getNextValue () const;
};

}

#endif /* BWIDGETS_BITEMS_HPP_ */
