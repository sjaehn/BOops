/* mix.hpp
 * Copyright (C) 2020  Sven Jähnichen
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

#ifndef BUTILITIES_MIX_HPP_
#define BUTILITIES_MIX_HPP_

namespace BUtilities {

template <class T> inline T mix (const T& t0, const T& t1, const double ratio)
{
        return t1 * ratio + t0 * (1.0f - ratio);
}

}

#endif /* BUTILITIES_MIX_HPP_ */
