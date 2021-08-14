/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
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

#ifndef BOOL2HSTR_HPP_
#define BOOL2HSTR_HPP_

#include <cstdint>
#include <cstring>
#include <string>

template <class T> void bool2hstr (const T& bools, char* dest, size_t sz = std::string::npos)
{
	// TODO optimize algorithm
	const char* hexstr = "0123456789ABCDEF";
	if (sz == std::string::npos) sz = bools.size();
	dest[0] = 0;
	for (size_t i = 0; i * 4 < sz; ++i)
	{
		uint8_t quad = 0;
		for (size_t j = 0; (j < 4) && (j + 4 * i < sz); ++j) quad += bools[j + 4 * i] * (1 << j);
		memmove (dest + 1, dest, i + 1);
		dest[0] = hexstr[quad];
	}
}

template <class T> void hstr2bool (const char* source, T& bools)
{
	// TODO optimize algorithm
	const char* hexstr = "0123456789ABCDEF";
	const size_t sz = strlen (source);
	for (size_t i = 0; i < sz; ++i)
	{
		const size_t pos = sz - i - 1;
		const char* iptr = strchr (hexstr, source[pos]);
		if (!iptr) 
		{
			for (size_t j = 0; (j < 4) && (4 * i + j < bools.size()); ++j) bools[4 * i + j] = false;
		}

		else
		{
			const size_t quad = iptr - hexstr;
			for (size_t j = 0; (j < 4) && (4 * i + j < bools.size()); ++j) bools[4 * i + j] = bool (quad & (1 << j));
		}
	}
}

#endif /* BOOL2HSTR_HPP_ */