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

#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cmath>
// #include <iostream>
#include "BUtilities/Point.hpp"
#include "Node.hpp"
#include "StaticArrayList.hpp"

#define MAPRES 1024

template<size_t sz>
class Shape
{
public:
	Shape ();
	Shape (const StaticArrayList<Node, sz> nodes, double transformFactor = 1.0, double transformOffset = 0.0);
	virtual ~Shape ();

	bool operator== (const Shape<sz>& rhs);
	bool operator!= (const Shape<sz>& rhs);

	void setTransformation (const double transformFactor, const double transformOffset);
	virtual void clearShape ();
	virtual void setDefaultShape ();
	bool isDefault () const;
	size_t size () const;
	Node getRawNode (const size_t nr) const;
	Node getNode (const size_t nr) const;
	size_t findRawNode (const Node& node);

	bool validateNode (const size_t nr);
	bool validateShape ();

	bool appendRawNode (const Node& node);
	bool appendNode (const Node& node);
	bool insertRawNode (const size_t pos, const Node& node);
	bool insertNode (const size_t pos, const Node& node);
	bool insertRawNode (const Node& node);
	bool insertNode (const Node& node);
	bool changeRawNode (const size_t pos, const Node& newnode);
	bool changeNode (const size_t pos, const Node& newnode);
	bool deleteNode (const size_t pos);

	double getMapRawValue (const double x) const;
	double getMapValue (const double x) const;
	double* getMap ();

protected:
	double transform (const double value) const;
	double retransform (const double value) const;
	Node transformNode (const Node& node) const;
	Node retransformNode (const Node& node) const;
	virtual void drawLineOnMap (const BUtilities::Point p1, const BUtilities::Point p2);
	BUtilities::Point getPointPerc (const BUtilities::Point p1, const BUtilities::Point p2, const double perc) const;
	virtual void renderBezier (const Node& n1, const Node& n2);

	StaticArrayList<Node, sz> nodes_;
	double map_[MAPRES];
	double factor_;
	double offset_;

};

template<size_t sz> Shape<sz>::Shape () : nodes_ (), map_ {0.0}, factor_ (1.0), offset_ (0.0) {}

template<size_t sz> Shape<sz>::Shape (const StaticArrayList<Node, sz> nodes, double transformFactor, double transformOffset) :
nodes_ (nodes), map_ {0.0}, factor_ (transformFactor), offset_ (transformFactor) {}

template<size_t sz> Shape<sz>::~Shape () {}

template<size_t sz> bool Shape<sz>::operator== (const Shape<sz>& rhs)
{
	if (size () != rhs.size ()) return false;
	for (int i = 0; i < size (); ++i) if (nodes_[i] != rhs.nodes_[i]) return false;
	return true;
}

template<size_t sz> bool Shape<sz>::operator!= (const Shape<sz>& rhs) {return !(*this == rhs);}

template<size_t sz> void Shape<sz>::setTransformation (const double transformFactor, const double transformOffset)
{
	// Prevent div by zero
	if (transformFactor == 0.0) return;

	factor_ = transformFactor;
	offset_ = transformOffset;
}

template<size_t sz> void Shape<sz>::clearShape ()
{
	while (!nodes_.empty ()) nodes_.pop_back ();
	for (int i = 0; i < MAPRES; ++i) map_[i] = 0;
}

template<size_t sz> void Shape<sz>::setDefaultShape ()
{
	clearShape ();
	nodes_.push_back ({NodeType::END_NODE, {0, 0}, {0, 0}, {0, 0}});
	nodes_.push_back ({NodeType::END_NODE, {1, 0}, {0, 0}, {0, 0}});
	renderBezier (nodes_[0], nodes_[1]);
}

template<size_t sz>bool Shape<sz>::isDefault () const
{
	return ((nodes_.size == 2) && (nodes_[0] == Node {NodeType::END_NODE, {0, 0}, {0, 0}, {0, 0}}));
}

template<size_t sz>size_t Shape<sz>::size () const {return nodes_.size;}

template<size_t sz>Node Shape<sz>::getRawNode (const size_t nr) const {return nodes_[nr];}

template<size_t sz>Node Shape<sz>::getNode (const size_t nr) const {return retransformNode (getRawNode (nr));}

template<size_t sz>size_t Shape<sz>::findRawNode (const Node& node)
{
	for (int i = 0; i < nodes_.size; ++i)
	{
		if (nodes_[i] == node) return i;
	}
	return nodes_.size;
}

template<size_t sz> bool Shape<sz>::appendRawNode (const Node& node)
{
	if (nodes_.size < sz)
	{
		nodes_.push_back (node);
		return true;
	}

	return false;
}

template<size_t sz> bool Shape<sz>::appendNode (const Node& node)
{
	return appendRawNode (transformNode (node));
}

template<size_t sz> bool Shape<sz>::insertRawNode (const size_t pos, const Node& node)
{
	// Nodes full => errorNode
	if (nodes_.size >= sz) return false;

	size_t p = pos;

	// Insert
	if (p < nodes_.size) nodes_.insert (nodes_.begin() + p, node);

	// Otherwise append
	else
	{
		p = nodes_.size;
		nodes_.push_back (node);
	}

	// Validate node and its neighbors
	if (!validateNode (p)) return false;
	if ((p > 0) && (!validateNode (p - 1))) return false;
	if ((p + 1 < nodes_.size) && (!validateNode (p + 1))) return false;

	// Update map
	for (unsigned int i = (p >= 2 ? p - 2 : 0); (i <= p + 1) && (i + 1 < nodes_.size); ++ i) renderBezier (nodes_[i], nodes_[i + 1]);
	return true;
}

template<size_t sz> bool Shape<sz>::insertNode (const size_t pos, const Node& node)
{
	return insertRawNode (pos, transformNode (node));
}

template<size_t sz> bool Shape<sz>::insertRawNode (const Node& node)
{
	// Find position
	size_t pos = nodes_.size;
	for (unsigned int i = 0; i < nodes_.size; ++i)
	{
		if (node.point.x < nodes_[i].point.x)
		{
			// Add if not redundant
			if ((i > 0) && (node != nodes_[i - 1]))
			{
				pos = i;
				break;
			}

			else return false;
		}
	}

	// Insert
	return insertRawNode (pos, node);
}

template<size_t sz> bool Shape<sz>::insertNode (const Node& node)
{
	return insertRawNode (transformNode (node));
}

template<size_t sz> bool Shape<sz>::changeRawNode (const size_t pos, const Node& node)
{
	if (pos >= nodes_.size) return false;
	nodes_[pos] = node;

	// Validate node and its neighbors
	if (!validateNode (pos)) return false;
	if ((pos > 0) && (!validateNode (pos - 1))) return false;
	if ((pos + 1 < nodes_.size) && (!validateNode (pos + 1))) return false;

	// Update map
	for (unsigned int i = (pos >= 2 ? pos - 2 : 0); (i <= pos + 1) && (i + 1 < nodes_.size); ++i) renderBezier (nodes_[i], nodes_[i + 1]);

	return true;
}

template<size_t sz> bool Shape<sz>::changeNode (const size_t pos, const Node& node)
{
	return changeRawNode (pos, transformNode (node));
}

template<size_t sz> bool Shape<sz>::deleteNode (const size_t pos)
{
	// Only deletion of middle nodes allowed
	if ((pos == 0) || (pos >= nodes_.size - 1)) return false;

	nodes_.erase (nodes_.begin() + pos);

	// Validate neighbor nodes
	if (!validateNode (pos - 1)) return false;
	if (!validateNode (pos)) return false;

	// Update map
	for (unsigned int i = (pos >= 2 ? pos - 2 : 0); (i <= pos) && (i + 1 < nodes_.size); ++ i) renderBezier (nodes_[i], nodes_[i + 1]);
	return true;
}

template<size_t sz> bool Shape<sz>::validateNode (const size_t nr)
{
	// Exception: Invalid parameters
	if (nr >= nodes_.size)
	{
		fprintf (stderr, "BOops.lv2: Node validation called with invalid parameters (node: %li).\n", nr);
		return false;
	}

	// Exception: Invalid node order
	if ((nodes_.size >= 3) && (nr > 1) && (nr < nodes_.size - 1) && (nodes_[nr-1].point.x > nodes_[nr+1].point.x))
	{
		fprintf (stderr, "BOops.lv2: Corrupt node data at node %li (%f, %f). Reset shape.\n", nr, nodes_[nr].point.x, nodes_[nr].point.y);
		setDefaultShape ();
		return false;
	}

	// Start node
	if (nr == 0)
	{
		// Check: Only end nodes on start position
		if  (nodes_[0].nodeType != NodeType::END_NODE) nodes_[0] = {NodeType::END_NODE, {0, 0}, {0, 0}, {0, 0}};

		// Check: Start position
		if (nodes_[0].point.x != 0) nodes_[0].point.x = 0;

		// Check: No handles
		nodes_[0].handle1 = BUtilities::Point (0, 0);
		nodes_[0].handle2 = BUtilities::Point (0, 0);
	}

	// End node
	else if (nr == nodes_.size - 1)
	{
		// Check: Only end nodes on end position
		if (nodes_[nr].nodeType != NodeType::END_NODE)
		{
			nodes_[nr] = {NodeType::END_NODE, {1, nodes_[0].point.y}, {0, 0}, {0, 0}};
		}

		// Check: No handles
		nodes_[nr].handle1 = BUtilities::Point (0, 0);
		nodes_[nr].handle2 = BUtilities::Point (0, 0);
	}

	// Middle nodes
	else
	{
		// Check: No end nodes in the middle
		if (nodes_[nr].nodeType == NodeType::END_NODE) nodes_[nr].nodeType = NodeType::CORNER_NODE;

		// Check: Nodes point order
		if (nodes_[nr].point.x < nodes_[nr - 1].point.x) nodes_[nr].point.x = nodes_[nr - 1].point.x;
		if (nodes_[nr].point.x > nodes_[nr + 1].point.x) nodes_[nr].point.x = nodes_[nr + 1].point.x;

		// Check: POINT_NODE without handles
		if (nodes_[nr].nodeType == NodeType::POINT_NODE)
		{
			nodes_[nr].handle1 = BUtilities::Point (0, 0);
			nodes_[nr].handle2 = BUtilities::Point (0, 0);
		}

		// Check: Handles order
		if (nodes_[nr].handle1.x > 0) nodes_[nr].handle1.x = 0;
		if (nodes_[nr].handle2.x < 0) nodes_[nr].handle2.x = 0;

		// Check: AUTO_SMOOTH_NODE with symmetric handles with the half size of the distance to the closest neighbor point
		if (nodes_[nr].nodeType == NodeType::AUTO_SMOOTH_NODE)
		{
			double dist = (nodes_[nr].point.x - nodes_[nr-1].point.x > nodes_[nr+1].point.x - nodes_[nr].point.x ?
						   nodes_[nr+1].point.x - nodes_[nr].point.x :
						   nodes_[nr].point.x - nodes_[nr-1].point.x);
			double ydist = (nodes_[nr + 1].point.y - nodes_[nr - 1].point.y);
			double yamp = (fabs (nodes_[nr - 1].point.y != 0) && fabs (nodes_[nr + 1].point.y != 0) ?
								(fabs (nodes_[nr - 1].point.y) < fabs (nodes_[nr + 1].point.y) ?
										fabs (nodes_[nr - 1].point.y) / (fabs (nodes_[nr - 1].point.y) + fabs (nodes_[nr + 1].point.y)) :
										fabs (nodes_[nr + 1].point.y) / (fabs (nodes_[nr - 1].point.y) + fabs (nodes_[nr + 1].point.y))) :
								0);
			nodes_[nr].handle1.x = -dist / 2;
			nodes_[nr].handle1.y = -ydist * yamp;
			nodes_[nr].handle2.x = dist / 2;
			nodes_[nr].handle2.y = ydist * yamp;
		}

		// Check: SYMMETRIC_SMOOTH_NODE must be symmetric
		else if (nodes_[nr].nodeType == NodeType::SYMMETRIC_SMOOTH_NODE)
		{
			//Check if handle1 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle1.x < nodes_[nr-1].point.x)
			{
				double f = (nodes_[nr-1].point.x - nodes_[nr].point.x) / nodes_[nr].handle1.x;
				nodes_[nr].handle1.x *= f;
				nodes_[nr].handle1.y *= f;
			}

			// Make handele2 symmetric to handle1
			nodes_[nr].handle2 = BUtilities::Point (0, 0) - nodes_[nr].handle1;

			//Check if handle2 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle2.x > nodes_[nr+1].point.x)
			{
				double f = (nodes_[nr+1].point.x - nodes_[nr].point.x) / nodes_[nr].handle2.x;
				nodes_[nr].handle2.x *= f;
				nodes_[nr].handle2.y *= f;
				nodes_[nr].handle1 = BUtilities::Point (0, 0) - nodes_[nr].handle2;
			}
		}

		// Check: SMOOTH_NODE handles point to opposite directions
		else if (nodes_[nr].nodeType == NodeType::SMOOTH_NODE)
		{
			//Check if handle1 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle1.x < nodes_[nr-1].point.x)
			{
				double f = (nodes_[nr-1].point.x - nodes_[nr].point.x) / nodes_[nr].handle1.x;
				nodes_[nr].handle1.x *= f;
				nodes_[nr].handle1.y *= f;
			}

			// Calculate handle distances
			double dist1 = sqrt (nodes_[nr].handle1.x * nodes_[nr].handle1.x + nodes_[nr].handle1.y * nodes_[nr].handle1.y);
			double dist2 = sqrt (nodes_[nr].handle2.x * nodes_[nr].handle2.x + nodes_[nr].handle2.y * nodes_[nr].handle2.y);

			// Recalculate handle2
			if ((dist1 != 0) && (dist2 != 0))
			{
				nodes_[nr].handle2.x = -(nodes_[nr].handle1.x * dist2 / dist1);
				nodes_[nr].handle2.y = -(nodes_[nr].handle1.y * dist2 / dist1);
			}

			//Check if handle2 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle2.x > nodes_[nr+1].point.x)
			{
				double f = (nodes_[nr+1].point.x - nodes_[nr].point.x) / nodes_[nr].handle2.x;
				nodes_[nr].handle2.x *= f;
				nodes_[nr].handle2.y *= f;
			}
		}

		// Check: CORNER_NODE
		else if (nodes_[nr].nodeType == NodeType::CORNER_NODE)
		{
			//Check if handle1 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle1.x < nodes_[nr-1].point.x)
			{
				double f = (nodes_[nr-1].point.x - nodes_[nr].point.x) / nodes_[nr].handle1.x;
				nodes_[nr].handle1.x *= f;
				nodes_[nr].handle1.y *= f;
			}

			//Check if handle2 overlaps neighbor point
			if (nodes_[nr].point.x + nodes_[nr].handle2.x > nodes_[nr+1].point.x)
			{
				double f = (nodes_[nr+1].point.x - nodes_[nr].point.x) / nodes_[nr].handle2.x;
				nodes_[nr].handle2.x *= f;
				nodes_[nr].handle2.y *= f;
			}
		}
	}

	return true;
}

template<size_t sz> bool Shape<sz>::validateShape ()
{
	// TODO Sort ???

	// Validate nodes
	bool status = true;
	for (unsigned int i = 0; i < nodes_.size; ++i)
	{
		if (!validateNode (i)) status = false;
	}

	// Update map
	for (unsigned int i = 0; i + 1 < nodes_.size; ++i) renderBezier (nodes_[i], nodes_[i+1]);

	return status;
}

template<size_t sz> double Shape<sz>::transform (const double value) const
{
	return (value - offset_) / factor_;
}

template<size_t sz> double Shape<sz>::retransform (const double value) const
{
	return factor_ * value + offset_;
}

template<size_t sz> Node Shape<sz>::transformNode (const Node& node) const
{
	return Node
	(
		node.nodeType,
		BUtilities::Point (node.point.x, transform (node.point.y)),
		BUtilities::Point (node.handle1.x, node.handle1.y / factor_),
		BUtilities::Point (node.handle2.x, node.handle2.y / factor_)
	);
}

template<size_t sz> Node Shape<sz>::retransformNode (const Node& node) const
{
	return Node
	(
		node.nodeType,
		BUtilities::Point (node.point.x, retransform (node.point.y)),
		BUtilities::Point (node.handle1.x, node.handle1.y * factor_),
		BUtilities::Point (node.handle2.x, node.handle2.y * factor_)
	);
}

template<size_t sz> void Shape<sz>::drawLineOnMap (BUtilities::Point p1, BUtilities::Point p2)
{
	if (p1.x < p2.x)
	{
		for (double x = p1.x; (x <= p2.x) && (x <= 1.0); x += (1.0 / MAPRES))
		{
			uint32_t i = ((uint32_t) (x * MAPRES)) % MAPRES;
			map_[i] = p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
		}
	}

	else
	{
		uint32_t i = ((uint32_t) (p2.x * MAPRES)) % MAPRES;
		map_ [i] = p2.y;
	}
}

template<size_t sz> BUtilities::Point Shape<sz>::getPointPerc (const BUtilities::Point p1, const BUtilities::Point p2 , const double perc) const
{
	BUtilities::Point p;
	p.x = p1.x + (p2.x - p1.x) * perc;
	p.y = p1.y + (p2.y - p1.y) * perc;
	return p;
}

template<size_t sz> void Shape<sz>::renderBezier (const Node& n1, const Node& n2)
{
	// Interpolate Bezier curve
	BUtilities::Point p1 = n1.point;
	BUtilities::Point p2 = n1.point + n1.handle2;
	BUtilities::Point p4 = n2.point;
	BUtilities::Point p3 = n2.point + n2.handle1;
	BUtilities::Point py = p1;
	double step = 1 / (fabs (n2.point.x - n1.point.x) * MAPRES + 1);

	for (double t = 0; t < 1; t += step)
	{
	    BUtilities::Point pa = getPointPerc (p1, p2, t);
	    BUtilities::Point pb = getPointPerc (p2, p3, t);
	    BUtilities::Point pc = getPointPerc (p3, p4, t);
	    BUtilities::Point pm = getPointPerc (pa, pb, t);
	    BUtilities::Point pn = getPointPerc (pb, pc, t);
	    BUtilities::Point pz = getPointPerc (pm, pn, t);

	    drawLineOnMap (py, pz);
	    py = pz;
	}
	drawLineOnMap (py,p4);
}

template<size_t shapesize> double Shape<shapesize>::getMapRawValue (const double x) const
{
	double mapx = fmod (x * MAPRES, MAPRES);
	double xmod = mapx - int (mapx);

	return (1 - xmod) * map_[int (mapx)] + xmod * map_[int (mapx + 1) % MAPRES];
}

template<size_t shapesize> double Shape<shapesize>::getMapValue (const double x) const
{
	return retransform (getMapRawValue (x));
}

template<size_t sz> double* Shape<sz>::getMap () {return &map_[0];}

/*
template<size_t sz> std::ostream &operator<<(std::ostream &output, Shape<sz>& shape)
{
	output << shape.nodes;
	return output;
}
*/
#endif /* SHAPE_HPP_ */
