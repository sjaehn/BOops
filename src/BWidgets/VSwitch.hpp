/* VSwitch.hpp
 * Copyright (C) 2018  Sven Jähnichen
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

#ifndef BWIDGETS_VSWITCH_HPP_
#define BWIDGETS_VSWITCH_HPP_

#include "Knob.hpp"
#include "VSlider.hpp"

#define BWIDGETS_DEFAULT_VSWITCH_WIDTH 40.0
#define BWIDGETS_DEFAULT_VSWITCH_HEIGHT 20.0
#define BWIDGETS_DEFAULT_VSWITCH_DEPTH 1.0

namespace BWidgets
{
/**
 * Class BWidgets::VSwitch
 *
 * On/OFF switch widget. Is is a BWidgets::VSlider having two conditions: on
 * (value != 0) or off (value == 0)
 */
class VSwitch : public VSlider
{
public:
	VSwitch ();
	VSwitch (const double x, const double y, const double width, const double height, const std::string& name, const double defaultvalue);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

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

protected:
	virtual void updateCoords () override;

	bool dragged;
};

}

#endif /* BWIDGETS_VSWITCH_HPP_ */
