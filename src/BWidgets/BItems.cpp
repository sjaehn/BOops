/* BItems.cpp
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

#include "BItems.hpp"
#include "Label.hpp"
#include <exception>

namespace BItems
{

Item::Item () : Item (UNSELECTED, nullptr) {}

Item::Item (const double value, BWidgets::Widget* widget) :
        value (value), widget (widget), internal (nullptr) {}

Item::Item (const double value, const std::string& text) :
        Item (value, nullptr)
{
        try {internal = new BWidgets::Label (0, 0, BWIDGETS_DEFAULT_LABEL_WIDTH, BWIDGETS_DEFAULT_LABEL_HEIGHT, "label", text);}
        catch (std::bad_alloc &ba) {throw ba;}
        internal->getBorder ()->setPadding (BITEMS_DEFAULT_TEXT_PADDING);
        widget = internal;
}

Item::Item (const Item& that) :
        Item (that.value, nullptr)
{
        if (that.internal)
        {
                try {internal = that.internal->clone ();}
                catch (std::bad_alloc &ba) {throw ba;}
                widget = internal;
        }
        else
        {
                internal = nullptr;
                widget = that.widget;
        }
}

Item::~Item ()
{
        if (internal) deleteInternal ();
}

void Item::deleteInternal ()
{
        delete internal;
        internal = nullptr;
        widget = nullptr;
}

Item& Item::operator= (const Item& that)
{
        value = that.value;

        if (internal) deleteInternal ();
        if (that.internal)
        {
                try {internal = that.internal->clone ();}
                catch (std::bad_alloc &ba) {throw ba;}
                widget = internal;
        }
        else
        {
                internal = nullptr;
                widget = that.widget;
        }

        return *this;
}

void Item::setValue (const double value) {this->value = value;}

double Item::getValue () const {return this->value;}

void Item::setWidget (BWidgets::Widget* widget)
{
        if (internal) deleteInternal ();
        this->widget = widget;
}

void Item::setWidget (const std::string& text)
{
        if (internal) deleteInternal ();
        try {internal = new BWidgets::Label (0, 0, BWIDGETS_DEFAULT_LABEL_WIDTH, BWIDGETS_DEFAULT_LABEL_HEIGHT, "label", text);}
        catch (std::bad_alloc &ba) {throw ba;}
        internal->getBorder ()->setPadding (BITEMS_DEFAULT_TEXT_PADDING);
        widget = internal;
}

void Item::cloneWidgetFrom (BWidgets::Widget* widget)
{
        if (internal) deleteInternal ();
        try {internal = widget->clone ();}
        catch (std::bad_alloc &ba) {throw ba;}
        this->widget = internal;
}

BWidgets::Widget* Item::getWidget () const {return widget;}

/*****************************************************************************/

ItemList::ItemList () : std::list<Item> () {}

ItemList::ItemList (const Item& item) : std::list<Item> ({item}) {}

ItemList::ItemList (const std::list<Item>& items) :
        std::list<Item> (items) {}

ItemList::ItemList (BWidgets::Widget* widget) :
        std::list<Item> ({Item (1.0, widget)}) {}

ItemList::ItemList (const std::initializer_list<BWidgets::Widget*>& widgets) :
        std::list<Item> ()
{
        for (BWidgets::Widget* w : widgets) push_back (Item (getNextValue (), w));;
}

ItemList::ItemList (const std::string& text) :
        std::list<Item> ({Item (1.0, text)}) {}

ItemList::ItemList (const std::initializer_list<std::string>& texts) :
        std::list<Item> ()
{
        for (std::string const& s : texts) push_back (Item (getNextValue (), s));
}

void ItemList::push_back (const Item& item) {std::list<Item>::push_back (item);}

void ItemList::push_back (BWidgets::Widget* widget)
{
        double v = getNextValue ();
        std::list<Item>::push_back (Item (v, widget));
}

void ItemList::push_back (const std::string& text)
{
        double v = getNextValue ();
        std::list<Item>::push_back (Item (v, text));  // TODO try catch bad_alloc
}

Item* ItemList::getItem (const double value)
{
        for (iterator it = begin (); it != end () ; ++it)
        {
                if ((*it).getValue () == value) return &(*it);
        }

        return nullptr;
}

double ItemList::getNextValue () const
{
        double maxValue = UNSELECTED;
        for (Item const& i : *this)
        {
                if (i.getValue () > maxValue) maxValue = i.getValue ();
        }
        if (maxValue != UNSELECTED) return floor (maxValue) + 1;
        else return 1.0;
}

}
