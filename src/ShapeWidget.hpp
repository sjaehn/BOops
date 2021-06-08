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

#ifndef SHAPEWIDGET_HPP_
#define SHAPEWIDGET_HPP_

#include <functional>
#include "Definitions.hpp"
#include "StaticArrayList.hpp"
#include "BWidgets/ValueWidget.hpp"
#include "BWidgets/Focusable.hpp"
#include "BWidgets/Text.hpp"
#include "Shape.hpp"
#include "Selection.hpp"
#include "Snapshots.hpp"

enum ToolType
{
	NO_TOOL				= 0,
	EDIT_TOOL			= 1,
	POINT_NODE_TOOL			= 2,
	AUTO_SMOOTH_NODE_TOOL		= 3,
	SYMMETRIC_SMOOTH_NODE_TOOL	= 4,
	CORNER_NODE_TOOL		= 5,
	NR_TOOLS			= 6,
};

class ShapeWidget : public Shape<SHAPE_MAXNODES>, public BWidgets::ValueWidget, public BWidgets::Focusable
{
public:
	ShapeWidget ();
	ShapeWidget (const double x, const double y, const double width, const double height, const std::string& name);
	ShapeWidget (const ShapeWidget& that);
	virtual BWidgets::Widget* clone () const override;
	virtual void update () override;
	void setTool (const ToolType tool);
	void setValueEnabled (const bool status);
	void setScaleParameters (double anchorYPos, double anchorValue, double ratio);
	void setMinorXSteps (double stepSize);
	void setMajorXSteps (double stepSize);
	void setYValueFunction (std::function<double (double x)> func);
	void setPrefix (std::string text);
	void setUnit (std::string text);
	void setLowerLimit (double value, bool hard = false);
	void setHigherLimit (double value, bool hard = false);
	void showGrid ();
	void hideGrid ();
	void setSnap (const bool status);
	std::vector<Node> cutSelection ();
	std::vector<Node> copySelection ();
	void pasteSelection (const std::vector<Node>& newNodes);
	void deleteSelection ();
	void unselect ();
	void reset ();
	void undo ();
	void redo ();
	void pushToSnapshots ();
	void resetSnapshots ();
	virtual void setDefaultShape () override;
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;
	virtual void onButtonReleased (BEvents::PointerEvent* event) override;
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;
	virtual void onValueChanged (BEvents::ValueChangedEvent* event) override;
	virtual void onFocusIn (BEvents::FocusEvent* event) override;
	virtual void onFocusOut (BEvents::FocusEvent* event) override;
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

protected:
	enum ClickMode
	{
		NEW_NODE	= 0,
		DRAG_NODE	= 1,
		DRAG_HANDLE	= 2,
		DRAG_SELECTION	= 3,
		DRAG_SCREEN	= 4
	};

	ClickMode clickMode;
	Selection selection;
	ToolType tool;
	int grabbedNode;
	int grabbedHandle;
	bool valueEnabled;
	double scaleAnchorYPos;
	double scaleAnchorValue;
	double scaleRatio;
	double minorXSteps;
	double majorXSteps;
	double loLimit, hiLimit;
	bool hardLoLimit, hardHiLimit;	// TODO, not in use yet
	bool gridVisible;
	bool gridSnap;
	std::function<double (double x)> func;
	std::string prefix;
	std::string unit;
	BColors::ColorSet fgColors;
	BColors::ColorSet syColors;
	BColors::ColorSet bgColors;
	BStyles::Font lbfont;

	BWidgets::Text focusText;

	double snapX (const double x);
	double snapY (const double y);

	Snapshots<Shape<SHAPE_MAXNODES>, MAXUNDO> undoSnapshots;

	virtual void drawLineOnMap (BUtilities::Point p1, BUtilities::Point p2) override;
	virtual void draw (const BUtilities::RectArea& area) override;
};

#endif /* SHAPEWIDGET_HPP_ */
