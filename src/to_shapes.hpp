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

#ifndef TO_SHAPES_HPP_
#define TO_SHAPES_HPP_

#include <string>
#include <array>
#include "BUtilities/stof.hpp"
#include "Definitions.hpp"
#include "Ports.hpp"
#include "Shape.hpp"

void to_shapes (const std::string& text, std::array<Shape<SHAPE_MAXNODES>, NR_SLOTS>& shapes)
{
    const std::string keywords[8] = {"slo:", "typ:", "ptx:", "pty:", "h1x:", "h1y:", "h2x:", "h2y:"};
    std::string s = text;
    for (Shape<SHAPE_MAXNODES>& sh : shapes) sh.clearShape();

    while (!s.empty())
    {
        // Look for next "slo:"
        size_t sPos = s.find (keywords[0]);
        size_t nextSPos = 0;
        if (sPos == std::string::npos) break;	// No "shp:" found => end
        if (sPos + 4 > s.length()) break;		// Nothing more after id => end
        s.erase (0, sPos + 4);

        int sl;
        try {sl = BUtilities::stof (s, &nextSPos);}
        catch  (const std::exception& e)
        {
            fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Can't parse shape number from \"%s...\"", s.substr (0, 63).c_str());
            break;
        }

        if (nextSPos > 0) s.erase (0, nextSPos);
        if ((sl < 0) || (sl >= NR_SLOTS))
        {
            fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Invalid matrix data block loaded for shape %i.\n", sl);
            break;
        }

        // Look for shape data
        Node node = {NodeType::POINT_NODE, {0, 0}, {0, 0}, {0, 0}};
        bool isTypeDef = false;
        for (int i = 1; i < 8; ++i)
        {
            sPos = s.find (keywords[i]);
            if (sPos == std::string::npos) continue;	// Keyword not found => next keyword
            if (sPos + 4 >= s.length())	// Nothing more after keyword => end
            {
                s ="";
                break;
            }
            if (sPos > 0) s.erase (0, sPos + 4);
            float val;
            try {val = BUtilities::stof (s, &nextSPos);}
            catch  (const std::exception& e)
            {
                fprintf (stderr, "BOops.lv2: Restore shape state incomplete. Can't parse %s from \"%s...\"",
                        keywords[i].substr(0,3).c_str(), s.substr (0, 63).c_str());
                break;
            }

            if (nextSPos > 0) s.erase (0, nextSPos);
            switch (i)
            {
                case 1: node.nodeType = (NodeType)((int)val);
                    isTypeDef = true;
                    break;
                case 2: node.point.x = val;
                    break;
                case 3:	node.point.y = val;
                    break;
                case 4:	node.handle1.x = val;
                    break;
                case 5:	node.handle1.y = val;
                    break;
                case 6:	node.handle2.x = val;
                    break;
                case 7:	node.handle2.y = val;
                    break;
                default:break;
            }
        }

        // Set data
        shapes[sl].appendNode (node);
    }
}

#endif /* TO_SHAPES_HPP_ */
