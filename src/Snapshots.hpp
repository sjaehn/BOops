/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2019 by Sven Jähnichen
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

#ifndef SNAPSHOTS_HPP_
#define SNAPSHOTS_HPP_

#include <cstddef>
#include <array>
#include <iostream>

template <class T, size_t sz>
class Snapshots
{
protected:
        std::array<T, sz> store;
        size_t pos = 0;
        size_t horizon = 0;
        size_t size = 0;

public:
        void clear ()
        {
                store.fill (T());
                pos = 0;
                horizon = 0;
                size = 0;
        }
        void push (T& value)
        {
                horizon = ((pos + 1) % sz);
                store[horizon] = value;
                pos = horizon;
                size = (size < sz ? size + 1 : sz);
        }

        T undo ()
        {
                pos = (size == 0 ? 0 : (((size < sz) && (pos == 0)) ? 0 : ((horizon == ((pos + sz - 1) % sz)) ? pos : ((pos + sz - 1) % sz))));
                return store[pos];
        }

        T redo ()
        {
                pos = (size == 0 ? 0 : ((horizon == pos) ? pos : ((pos + 1) % sz)));
                return store[pos];
        }
};


#endif /* SNAPSHOTS_HPP_ */
