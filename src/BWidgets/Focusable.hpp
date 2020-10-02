/* Focusable.hpp
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

#ifndef BWIDGETS_FOCUSABLE_HPP_
#define BWIDGETS_FOCUSABLE_HPP_

#define BWIDGETS_DEFAULT_FOCUS_IN_MS 200
#define BWIDGETS_DEFAULT_FOCUS_OUT_MS 5000
#define BWIDGETS_DEFAULT_FOCUS_NAME "/focus"

#include <chrono>

namespace BWidgets
{

class Focusable
{
protected:
        std::chrono::milliseconds focusInMs;
	std::chrono::milliseconds focusOutMs;

public:
        Focusable (const std::chrono::milliseconds focusInMs, const std::chrono::milliseconds focusOutMs) :
                focusInMs (focusInMs), focusOutMs (focusOutMs) {}

        void setFocusInMilliseconds (const std::chrono::milliseconds ms) {focusInMs = ms;}

	std::chrono::milliseconds getFocusInMilliseconds () const {return focusInMs;}

	void setFocusOutMilliseconds (const std::chrono::milliseconds ms) {focusOutMs = ms;}

	std::chrono::milliseconds getFocusOutMilliseconds () const {return focusOutMs;}

        bool isFocusActive (const std::chrono::milliseconds diffMs) const {return ((diffMs >= focusInMs) && (diffMs < focusOutMs));}
};

}

#endif /*BWIDGETS_FOCUSABLE_HPP_*/
