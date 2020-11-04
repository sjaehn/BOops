/* B.Oops
 * Glitch effect sequencer LV2 plugin
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

#ifndef HALOTOGGLEBUTTON_HPP_
#define HALOTOGGLEBUTTON_HPP_

#include "HaloButton.hpp"

class HaloToggleButton : public HaloButton
{
public:
	HaloToggleButton () : HaloToggleButton (0, 0, 0, 0, "editwidget", "") {}

	HaloToggleButton (const double x, const double y, const double width, const double height, const std::string& name, const std::string& focusText) :
		HaloButton (x, y, width, height, name, focusText) {}

	virtual BWidgets::Widget* clone () const override {return new HaloToggleButton (*this);}

	virtual void onButtonPressed (BEvents::PointerEvent* event) override
	{
		setValue (getValue() == 1.0 ? 0.0 : 1.0);
		Widget::cbfunction_[BEvents::EventType::BUTTON_PRESS_EVENT] (event);
	}

	virtual void onButtonReleased (BEvents::PointerEvent* event) override
	{
		Widget::cbfunction_[BEvents::EventType::BUTTON_RELEASE_EVENT] (event);
	}
};

#endif /* HALOTOGGLEBUTTON_HPP_ */
