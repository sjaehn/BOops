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

#ifndef VALUESELECT_HPP_
#define VALUESELECT_HPP_

#include "BWidgets/RangeWidget.hpp"
#include "BWidgets/Label.hpp"
#include "UpClick.hpp"
#include "DownClick.hpp"

class ValueSelect : public BWidgets::RangeWidget
{
public:
	ValueSelect ();
	ValueSelect (const double  x, const double y, const double width, const double height, const std::string& name,
				 const double value, const double min, const double max, const double step);

	virtual void applyTheme (BStyles::Theme& theme) override;
	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;
	virtual void update () override;
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;
	virtual void onWheelScrolled (BEvents::WheelEvent* event) override;

protected:
	static void buttonPressedCallback (BEvents::Event* event);
	static void displayDraggedCallback (BEvents::Event* event);
	static void displayMessageCallback (BEvents::Event* event);

	UpClick upClick;
	DownClick downClick;
	BWidgets::Label display;
};

#endif /* VALUESELECT_HPP_ */
