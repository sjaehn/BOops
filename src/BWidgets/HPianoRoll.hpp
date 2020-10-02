/* HPianoRoll.hpp
 * Copyright (C) 2019  Sven Jähnichen
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

#ifndef BWIDGETS_HPIANOROLL_HPP_
#define BWIDGETS_HPIANOROLL_HPP_

#include "PianoWidget.hpp"
#include <cmath>

namespace BWidgets
{

/*
 * Piano roll widget
 */
class HPianoRoll : public PianoWidget
{
public:
	HPianoRoll ();
	HPianoRoll (const double x, const double y, const double width, const double height, const std::string& name);
	HPianoRoll (const double x, const double y, const double width, const double height, const std::string& name,
			const int startMidiKey, const int endMidiKey);

	/**
	 * Pattern cloning. Creates a new instance of the widget and copies all
	 * its properties.
	 */
	virtual Widget* clone () const override;

	/*
	 * Defines how the keys respond upon pointer events.
	 * @param toggle	True, if the key is toggled if button pressed. False,
	 * 					if the key is pressed if button pressed and released,
	 * 					if button released.
	 */
	void setKeysToggleable (const bool toggle);

	/*
	 * Gets how the keys respond upon pointer events.
	 * @param return	True, if the key is toggled if button pressed. False,
	 * 					if the key is pressed if button pressed and released,
	 * 					if button released.
	 */
	bool isKeysToggleable () const;

// TODO key colors
//	virtual void applyTheme (BStyles::Theme& theme, const std::string& name) override;

	/**
	 * Handles the BEvents::BUTTON_PRESS_EVENT to press the keys.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onButtonPressed (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::BUTTON_RELEASE_EVENT to release the keys. Takes
	 * only effect if keys are NOT toggleable.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onButtonReleased (BEvents::PointerEvent* event) override;

	/**
	 * Handles the BEvents::POINTER_DRAGGED_EVENT to press or release the keys.
	 * @param event Pointer to a pointer event emitted by the same widget.
	 */
	virtual void onPointerDragged (BEvents::PointerEvent* event) override;

protected:
	int getKey (const BUtilities::Point position);
	virtual void draw (const BUtilities::RectArea& area) override;

	bool toggleKeys;
	int actKeyNr;

	BColors::ColorSet blackBgColors;
	BColors::ColorSet whiteBgColors;
};
}

#endif /* BWIDGETS_HPIANOROLL_HPP_ */
