/* PianoWidget.hpp
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

#ifndef BWIDGETS_PIANOWIDGET_HPP_
#define BWIDGETS_PIANOWIDGET_HPP_

#include "Widget.hpp"

#define BWIDGETS_PIANO_KEY_PRESSED_MESSAGE "PIANO_KEY_PRESSED"
#define BWIDGETS_PIANO_KEY_RELEASED_MESSAGE "PIANO_KEY_RELEASED"

namespace BWidgets
{

#ifndef BWIDGETS_KEYCOORDS_
#define BWIDGETS_KEYCOORDS_
typedef struct {
	bool whiteKey;
	double x;
	double dx1;
	double dx2;
	double width;
} KeyCoords;

const std::array<KeyCoords, 12> keyCoords=
{{
		{true, 0.0, 0.0, 0.4167, 1.0},
		{false, 0.5833, 0.0, 0.0, 0.6667},
		{true, 1.0, 0.25, 0.25, 1.0},
		{false, 1.75, 0, 0, 0.6667},
		{true, 2, 0.4167, 0, 1},
		{true, 3, 0, 0.5, 1},
		{false, 3.5, 0, 0, 0.6667},
		{true, 4, 0.1667, 0.3333, 1},
		{false, 4.6667, 0, 0, 0.6667},
		{true, 5, 0.3333, 0.1667, 1},
		{false, 5.8333, 0, 0, 0.6667},
		{true, 6, 0.5, 0, 1}
}};
#endif /* BWIDGETS_KEYCOORDS_ */

/*
 * Class that handles keyboard widget specific data. This class doesn't
 * implement an own draw method and thus it doesn't draw a keyboard! Use
 * HPianoRoll or VPianoRoll instead.
 */
class PianoWidget : public Widget
{
public:
	PianoWidget ();
	PianoWidget (const double x, const double y, const double width, const double height, const std::string& name);
	PianoWidget (const double x, const double y, const double width, const double height, const std::string& name,
				 const int startMidiKey, const int endMidiKey);

 	/**
 	 * Pattern cloning. Creates a new instance of the widget and copies all
 	 * its properties.
 	 */
 	virtual Widget* clone () const override;

	/*
	 * Sets the press status of the respective keys.
	 * @param keys	Vector of bool with a size that represents the number of
	 * 				keys. The first vector element represents the first key.
	 * 				True = pressed and false = released.
	 */
	void pressKeys (std::vector<bool>& keys);

	/*
	 * Gets the press status of the respective keys.
	 * @param return	Vector of bool with a size that represents the number
	 * 					of keys. The first vector element represents the first
	 * 					key. True = pressed and false = released.
	 */
	std::vector<bool> getPressedKeys () const;

	/*
	 * Sets the activity status of the respective keys. Active keys are
	 * highlighted and respond to pointer events.
	 * @param keys	Vector of bool with a size that represents the number of
	 * 				keys. The first vector element represents the first key.
	 * 				True = active and false = inactive.
	 */
	void activateKeys (std::vector<bool>& keys);

	/*
	 * Gets the activity status of the respective keys. Active keys are
	 * highlighted and respond to pointer events.
	 * @param return	Vector of bool with a size that represents the number
	 * 					of keys. The first vector element represents the first
	 * 					key. True = active and false = inactive.
	 */
	std::vector<bool> getActiveKeys () const;

protected:

	int startMidiKey;
	int endMidiKey;
	std::vector<bool> activeKeys;
	std::vector<bool> pressedKeys;
};

}

#endif /* BWIDGETS_PIANOWIDGET_HPP_ */
