/* VScale.hpp
 * Copyright (C) 2018, 2019  Sven Jähnichen
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

#ifndef BWIDGETS_VSCALE_HPP_
#define BWIDGETS_VSCALE_HPP_

#include "RangeWidget.hpp"

#define BWIDGETS_DEFAULT_VSCALE_WIDTH 100.0
#define BWIDGETS_DEFAULT_VSCALE_HEIGHT 6.0
#define BWIDGETS_DEFAULT_VSCALE_DEPTH 1.0

namespace BWidgets
{
/**
 * Class BWidgets::VScale
 *
 * RangeWidget class for a simple vertical scale.
 * The Widget is clickable by default.
 */
class VScale : public RangeWidget
{
public:
	VScale ();
	VScale (const double x, const double y, const double width, const double height, const std::string& name,
			 const double value, const double min, const double max, const double step);

	/**
	 * Creates a new (orphan) scale and copies the scale properties from a
	 * source scale. This method doesn't copy any parent or child widgets.
	 * @param that Source scale
	 */
	VScale (const VScale& that);

	/**
	 * Assignment. Copies the scale properties from a source scale and keeps
	 * its name and its position within the widget tree. Emits an expose event
	 * if the widget is visible and a value changed event.
	 * @param that Source slider
	 */
	VScale& operator= (const VScale& that);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/**
	 * Calls a redraw of the widget and calls postRedisplay () if the the
	 * Widget is visible.
	 * This method should be called if the widgets properties are indirectly
	 * changed.
	 */
	virtual void update () override;

	/**
	 * Scans theme for widget properties and applies these properties.
	 * @param theme Theme to be scanned.
	 * 				Styles used are:
	 * 				BWIDGETS_KEYWORD_BORDER
	 * 				BWIDGETS_KEYWORD_BACKGROUND
	 * 				BWIDGETS_KEYWORD_FGCOLORS
	 * 				BWIDGETS_KEYWORD_BGCOLORS
	 * @param name Name of the BStyles::StyleSet within the theme to be
	 * 		  	   applied.
	 */
	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Handles the BEvents::BUTTON_PRESS_EVENT to move the slider.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::EventType::BUTTON_RELEASE_EVENT to move the slider.
	 * @param event Pointer event
	 */
	virtual void onButtonReleased (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::POINTER_DRAG_EVENT to move
	 * the slider.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::WHEEL_SCROLL_EVENT to turn
	 * the dial.
	 * @param event Pointer to a wheel event emitted by the same widget.
	 */
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;

protected:
	virtual void updateCoords ();
	virtual void draw (const BUtilities::RectArea& area) override;

	BColors::ColorSet fgColors;
	BColors::ColorSet bgColors;
	BUtilities::RectArea scaleArea;
	double scaleYValue;
};

}

#endif /* BWIDGETS_VSCALE_HPP_ */
