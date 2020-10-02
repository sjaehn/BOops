/* to_string.cpp
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

#include "to_string.hpp"
#include <sstream>

namespace BUtilities {

std::string to_string (const double value)
{
	std::ostringstream os;
	os << value;
	std::string str = os.str();
	return str;
}

std::string to_string (const double value, const std::string& format)
{
	char c[64];
	snprintf (c, 64, format.c_str (), value);
	std::string str = c;
	return c;
}

}
