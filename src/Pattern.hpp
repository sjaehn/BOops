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

#ifndef PATTERN_HPP_
#define PATTERN_HPP_

#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include "Definitions.hpp"
#include "Ports.hpp"
#include "PadMessage.hpp"
#include "Journal.hpp"
#include "Shape.hpp"
#include "BUtilities/Any.hpp"

struct Action
{
        Action () : row (-1), step (0), content () {}
        Action (const int row, const int step, const BUtilities::Any& content) :
                row (row),
                step (step),
                content (content)
        {}

        int row;
        int step;
        BUtilities::Any content;
};


class Pattern
{
public:
        void clear ();
        Pad getPad (const size_t row, const size_t step) const;
        void setPad (const size_t row, const size_t step, const Pad& pad);
        Shape<SHAPE_MAXNODES> getShape(const size_t row) const;
        void setShape (const size_t row, const Shape<SHAPE_MAXNODES>& shape);
        std::array<bool, NR_PIANO_KEYS + 1> getKeys (const size_t row) const;
        bool getKey (const size_t row, const size_t note) const;
        void setKeys (const size_t row, const std::array<bool, NR_PIANO_KEYS + 1>& ks);
        void setKey (const size_t row, const size_t note, const bool state);
        std::vector<Action> undo ();
        std::vector<Action> redo ();
        void store ();
        std::string toString (const std::array<std::string, 2 + sizeof (Pad) / sizeof (float)>& symbols) const;
        void fromString (const std::string& text, const std::array<std::string, 2 + sizeof (Pad) / sizeof (float)>& symbols);
private:
        Journal<std::vector<Action>, MAXUNDO> journal;
        std::array<std::array<Pad, NR_STEPS>, NR_SLOTS> pads;
        std::array<Shape<SHAPE_MAXNODES>, NR_SLOTS> shapes;
        std::array<std::array<bool, NR_PIANO_KEYS + 1>, NR_SLOTS> keys;
        struct
        {
                std::vector<Action> oldMessage;
                std::vector<Action> newMessage;
        } changes;
};

#endif /* PATTERN_HPP_ */
