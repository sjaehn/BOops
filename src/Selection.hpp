/* B.Shapr
 * Beat / envelope shaper LV2 plugin
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

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include <array>
#include <iostream>
#include "Definitions.hpp"
#include "BUtilities/Point.hpp"

class Selection : private std::array<bool, SHAPE_MAXNODES>
{
protected:
	BUtilities::Point p1, ext;

public:
	using std::array<bool, SHAPE_MAXNODES>::operator[];
	using std::array<bool, SHAPE_MAXNODES>::fill;
	using std::array<bool, SHAPE_MAXNODES>::at;

	void setOrigin (const BUtilities::Point point) {p1 = point;}

	BUtilities::Point getOrigin () const {return p1;}

	void setExtend (const BUtilities::Point extend) {ext = extend;}

	BUtilities::Point getExtend () const {return ext;}

	void clear ()
	{
		fill (false);
		setOrigin ({0,0});
		setExtend ({0,0});
	}

	size_t getSelectedAntecessor (const size_t pos) const
        {
                if (pos >= SHAPE_MAXNODES) return SHAPE_MAXNODES;

                for (int i = pos - 1; i >= 0; --i)
                {
                        if (this->at(i)) return i;
                }

                return SHAPE_MAXNODES;
        }

	size_t getSelectedSuccessor (const size_t pos) const
        {
                if (pos >= SHAPE_MAXNODES) return SHAPE_MAXNODES;

                for (int i = pos + 1; i < SHAPE_MAXNODES; ++i)
                {
                        if (this->at(i)) return i;
                }

                return SHAPE_MAXNODES;
        }

	size_t getUnselectedAntecessor (const size_t pos) const
        {
                if (pos >= SHAPE_MAXNODES) return SHAPE_MAXNODES;

                for (int i = pos - 1; i >= 0; --i)
                {
                        if (!this->at(i)) return i;
                }

                return SHAPE_MAXNODES;
        }

	size_t getUnselectedSuccessor (const size_t pos) const
        {
                if (pos >= SHAPE_MAXNODES) return SHAPE_MAXNODES;

                for (int i = pos + 1; i < SHAPE_MAXNODES; ++i)
                {
                        if (!this->at(i)) return i;
                }

                return SHAPE_MAXNODES;
        }
};

#endif /* SELECTION_HPP_ */
