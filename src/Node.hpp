/* B.Noname01
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 219, 2020 by Sven Jähnichen
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

#ifndef NODE_HPP_
#define NODE_HPP_

#include "BUtilities/Point.hpp"
// #include <iostream>

enum NodeType
{
	END_NODE		= 0,	// End positions, only point data are used
	POINT_NODE		= 1,	// Only point data are used
	AUTO_SMOOTH_NODE	= 2,	// Uses two handles, both are calculated automatically
	SYMMETRIC_SMOOTH_NODE	= 3, 	// Uses two handles, the second handle is set to be symmetric to the first one
	SMOOTH_NODE		= 4,	// Uses two handles, the second handle points to the opposite direction of the first one
	CORNER_NODE		= 5	// Uses two independent handles
};

struct Node
{
	NodeType nodeType;
	BUtilities::Point point;
	BUtilities::Point handle1;
	BUtilities::Point handle2;

	Node () : Node (END_NODE, {0, 0}, {0, 0}, {0,0}) {}
	Node (NodeType nodeType, BUtilities::Point point, BUtilities::Point handle1, BUtilities::Point handle2) :
		nodeType (nodeType), point (point), handle1 (handle1), handle2 (handle2) {}
	Node (float* data) : nodeType ((NodeType) data[0]), point ({data[1], data[2]}), handle1 ({data[3], data[4]}), handle2 ({data[5], data[6]}) {}

	friend bool operator== (const Node& lhs, const Node& rhs)
		{return ((lhs.nodeType == rhs.nodeType) && (lhs.point == rhs.point) && (lhs.handle1 == rhs.handle1) && (lhs.handle2 == rhs.handle2));}
	friend bool operator!= (const Node& lhs, const Node& rhs) {return !(lhs == rhs);}
};

/*
std::ostream &operator<<(std::ostream &output, Node& node)
{
	output << "{" << int (node.nodeType) << ", " << node.point << ", " << node.handle1 << ", " << node.handle2 << "}";
	return output;
}
*/
#endif /* NODE_HPP_ */
