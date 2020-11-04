/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2018, 2019 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef JOURNAL_HPP_
#define JOURNAL_HPP_

#include <cstddef>
#include <list>
#include <stdexcept>

template <class T, size_t sz>
class Journal
{
protected:
        std::list<T> oldList;
        std::list<T> newList;

        // Undo iterators
        typename std::list<T>::iterator oldIterator;
        typename std::list<T>::iterator newIterator;

public:
        Journal& operator= (const Journal& rhs)
        {
                oldList = rhs.oldList;
                newList = rhs.newList;
                oldIterator = oldList.begin();
                int od = 0;
                for (auto it = rhs.oldList.begin(); it !=rhs.oldIterator; ++it, ++od);
                std::advance (oldIterator, od);
                newIterator = newList.begin();
                int nd = 0;
                for (auto it = rhs.newList.begin(); it !=rhs.newIterator; ++it, ++nd);
                std::advance (newIterator, nd);
                return *this;
        }

        void clear ()
        {
                oldList.clear ();
                oldIterator = oldList.begin ();
                newList.clear ();
                newIterator = newList.begin ();
        }

        void push (T& oldValue, T& newValue)
        {
                // Shrink lists to position of the undo iterators
                if ((!oldList.empty ()) && (oldIterator != --oldList.end ())) oldList = std::list<T> (oldList.begin (), std::next (oldIterator));
                if ((!newList.empty ()) && (newIterator != --newList.end ())) newList = std::list<T> (newList.begin (), std::next (newIterator));

                // Append changes to the lists
                oldList.push_back (oldValue);
                newList.push_back (newValue);

                // Shrink list size if maximal size exceeded
                if (oldList.size () > sz) oldList.pop_front ();
                if (newList.size () > sz) newList.pop_front ();

                // Point undo iterators to the last element
                oldIterator = --oldList.end ();
                newIterator = --newList.end ();
        }

        T undo ()
        {
                if (oldList.empty () || newList.empty ()) throw std::out_of_range ("Can't undo empty journal.");

                T& value = *oldIterator;
                if (oldIterator != oldList.begin ()) --oldIterator;
                if (newIterator != newList.begin ()) --newIterator;
                return value;
        }

        T redo ()
        {
                if (oldList.empty () || newList.empty ()) throw std::out_of_range ("Can't redo empty journal.");

                if (oldIterator != --oldList.end ()) ++oldIterator;
                if (newIterator != --newList.end ()) ++newIterator;
                return *newIterator;
        }
};


#endif /* JOURNAL_HPP_ */
