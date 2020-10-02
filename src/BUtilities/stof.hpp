/* stof.hpp
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

#ifndef BUTILITIES_STOF_HPP_
#define BUTILITIES_STOF_HPP_

#include <string>
#include <stdexcept>

namespace BUtilities {

float stof (const std::string& str, size_t* idx = 0)
{
        const std::string numbers = "0123456789";
        bool isNumber = false;
        float sign = 1.0f;
        float predec = 0.0f;
        float dec = 0.0f;
        float decfac = 0.1f;
        size_t i = 0;

        // Ignore spaces before
        while (str[i] == ' ') ++i;

        // Check sign
        if ((str[i] == '+') || (str[i] == '-'))
        {
                if (str[i] == '-') sign = -1.0f;
                ++i;
        }

        // Interpret pre-decimal digits
        while ((str[i] != 0) && (numbers.find_first_of (str[i]) != std::string::npos))
        {
                predec = predec * 10.0f + str[i] - '0';
                ++i;
                isNumber = true;
        }

        // Check decimal sign
        if ((str[i] == '.') || (str[i] == ','))
        {
                ++i;

                // Interpret decimal digits
                while ((str[i] != 0) && (numbers.find_first_of (str[i]) != std::string::npos))
                {
                        dec += (str[i] - '0') * decfac;
                        decfac *= 0.1f;
                        ++i;
                        isNumber = true;
                }
        }

        // Communicate next position
        if (idx != nullptr) *idx = i;

        // Not a number: invalid argument exception
        if (!isNumber) throw std::invalid_argument (str + "is not a number");

        return sign * predec + dec;
}

}

#endif /* BUTILITIES_STOF_HPP_ */
